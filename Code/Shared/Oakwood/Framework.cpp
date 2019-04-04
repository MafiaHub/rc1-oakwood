#define OAK_INTERNAL
#include "Framework.hpp"

// Listings
#include "VehicleListing.hpp"
#include "PlayerModelListing.hpp"

GameMode *__gm = nullptr;

GameMode::GameMode(oak_api *mod) {
    this->mod = mod;
    __gm = this;

    mod->on_player_connected = [=](librg_event* evnt, librg_entity* entity, mafia_player* ped) {
        
        Player *player = new Player(entity);

        player->SetModel("Tommy.i3d");
        player->SetHealth(100);

        if (onPlayerConnected)
            onPlayerConnected(player);

        players.AddObject(player);
    };

    mod->on_player_disconnected = [=](librg_event* evnt, librg_entity* entity) {

        auto player = players.GetObjectByEntity(entity);

        if (!player) {
            printf("[OAKWOOD] Unregistered entity was just disconnected !");
            return;
        }

        if (onPlayerDisconnected)
            onPlayerDisconnected(player);

        players.RemoveObject(player);
        delete player;
    };

    mod->on_player_died = [=](librg_entity* entity, mafia_player* ped) {

        auto player = players.GetObjectByEntity(entity);

        if (!player) {
            printf("[OAKWOOD] Unregistered entity just died !");
            return;
        }

        if (onPlayerDied)
            onPlayerDied(player);
    };

    mod->on_player_hit = [=](librg_entity *attacker_ent, librg_entity *victim_ent, float damage) {
        auto attacker = players.GetObjectByEntity(attacker_ent);
        auto victim = players.GetObjectByEntity(victim_ent);

        if (attacker && victim && onPlayerHit)
            onPlayerHit(attacker, victim, damage);
    };

    mod->on_vehicle_destroyed = [=](librg_entity *vehicle_ent) {
        auto vehicle = vehicles.GetObjectByEntity(vehicle_ent);

        if (vehicle) {
            if (onVehicleDestroyed)
                onVehicleDestroyed(vehicle);

            vehicle->destroyedByGame = true;
            vehicles.RemoveObject(vehicle);
            delete vehicle;
        }
    };

    mod->on_player_chat = [=](librg_entity* entity, std::string msg) {
        auto player = players.GetObjectByEntity(entity);

        if (!player) {
            printf("[OAKWOOD] Unregistered entity sends message!");
            return false;
        }
        
        auto args = SplitStringByWhitespace(msg);

        bool is_handled = false;

        auto cmd = commands.find(args[0]);

        if (cmd != commands.end()) {
            is_handled = cmd->second(player, args);
        }
        else if (onPlayerChat) {
            is_handled = onPlayerChat(player, msg);
        }
        
        return is_handled;
    };

    mod->on_server_tick = [=]() {
        if (onServerTick)
            onServerTick();
    };
}

GameMode::~GameMode() {
    this->mod = nullptr;
}

b32 GameObject::CompareWith(librg_entity *entity)
{
    return this->entity == entity;
}

void GameMode::BroadcastMessage(std::string text, u32 color)
{
    mod->vtable.broadcast_msg_color(text.c_str(), color);
}

void GameMode::SendMessageToPlayer(std::string text, Player *receiver, u32 color)
{
    if (!receiver)
        return;

    if (!receiver->entity)
        return;

    mod->vtable.send_msg(text.c_str(), receiver->entity);
}

void GameMode::ChatPrint(std::string text)
{
    mod->vtable.chat_print(text.c_str());
}

void GameMode::SpawnWeaponDrop(zpl_vec3 position, std::string model, inventory_item item)
{
    mod->vtable.drop_spawn(position, (char *)model.c_str(), item);
}

Vehicle* GameMode::SpawnVehicle(zpl_vec3 pos, float angle, const std::string& model, b32 show_in_radar)
{
    auto rot = ComputeDirVector(angle);
    auto entity = __gm->mod->vtable.vehicle_spawn(pos, rot, (char*)model.c_str(), show_in_radar);
    auto vehicle = new Vehicle(entity);
    vehicles.AddObject(vehicle);

    return vehicle;
}

Vehicle *GameMode::SpawnVehicleByID(zpl_vec3 pos, float angle, int modelID)
{
    if (modelID < 0 || modelID >= (int)VehicleCatalogue.size())
        return nullptr;

    auto modelName = VehicleCatalogue.at(modelID).second.c_str();

    return SpawnVehicle(pos, angle, modelName);
}

void GameMode::SetOnPlayerConnected(std::function<void(Player*)> callback)
{
    onPlayerConnected = callback;
}

void GameMode::SetOnPlayerDisconnected(std::function<void(Player*)> callback)
{
    onPlayerDisconnected = callback;
}

void GameMode::SetOnPlayerDied(std::function<void(Player*)> callback)
{
    onPlayerDied = callback;
}

void GameMode::SetOnPlayerHit(std::function<void(Player*,Player*,float)> callback)
{
    onPlayerHit = callback;
}

void GameMode::SetOnPlayerChat(std::function<bool(Player*, std::string msg)> callback)
{
    onPlayerChat = callback;
}

void GameMode::SetOnServerTick(std::function<void()> callback)
{
    onServerTick = callback;
}

void GameMode::AddCommandHandler(std::string command, std::function<bool(Player*,std::vector<std::string>)> callback)
{
    commands[command] = callback;
}

std::string GameMode::ImplodeArgumentList(ArgumentList args)
{
    return implode(args, 1);
}

//
// Player
//

Player::Player(librg_entity *entity) : GameObject()
{
    this->entity = entity;
}

Player::~Player()
{
}

void Player::Spawn(zpl_vec3 pos)
{
    __gm->mod->vtable.player_spawn(entity, pos);
}

void Player::SetModel(std::string name)
{
    __gm->mod->vtable.player_set_model(entity, (char *)name.c_str());
}

void Player::SetModelByID(int modelID)
{
    if (modelID < 0 || modelID >= zpl_count_of(PlayerModelCatalogue))
        return;

    auto modelName = std::string(PlayerModelCatalogue[modelID]) + ".i3d";
    SetModel(modelName);
}

std::string Player::GetModel()
{
    auto ped = GetPedestrian();
    return std::string(ped->model);
}

std::string Player::GetName()
{
    auto ped = GetPedestrian();
    return std::string(ped->name);
}

void Player::SetPosition(zpl_vec3 position)
{
    __gm->mod->vtable.player_set_position(entity, position);
}

zpl_vec3 Player::GetPosition()
{
    return entity->position;
}

void Player::SetRotation(float angle)
{
    auto dir = ComputeDirVector(angle);
    __gm->mod->vtable.player_set_rotation(entity, dir);
}

float Player::GetRotation()
{
    auto player = (mafia_player*)(entity->user_data);
    if(player) {
        return  DirToRotation180(player->rotation);
    }

    return 0.0f;
}


void Player::AddItem(inventory_item *item)
{
    __gm->mod->vtable.player_inventory_add(entity, item);
}

void Player::ClearInventory()
{
    /* TODO */
    auto ped = GetPedestrian();

    for (size_t i = 0; i < 8; i++)
        ped->inventory.items[i] = { -1, 0, 0, 0 };
}

u32 Player::GetCurrentWeapon()
{
    auto ped = GetPedestrian();

    return ped->current_weapon_id;
}

void Player::Fadeout(bool fadeout, u32 duration, u32 color)
{
    __gm->mod->vtable.player_fadeout(entity, fadeout, duration, color);
}

void Player::SetCamera(zpl_vec3 pos, zpl_vec3 rot)
{
    __gm->mod->vtable.player_set_camera(entity, pos, rot);
}

void Player::UnlockCamera()
{
    __gm->mod->vtable.player_unlock_camera(entity);
}

void Player::PlayAnimation(std::string animation)
{
    __gm->mod->vtable.player_play_animation(entity, zpl_bprintf("%s.i3d", animation.c_str()));
}

void Player::SetHealth(f32 health)
{
    __gm->mod->vtable.player_set_health(entity, health * 2.0f);
}

f32 Player::GetHealth()
{
    auto ped = GetPedestrian();
    return ped->health / 2.0f;
}

void Player::Die()
{
    __gm->mod->vtable.player_die(entity);
}

Vehicle *Player::GetVehicle()
{
    auto vehicle_ent = __gm->mod->vtable.player_get_vehicle(this->entity);

    if (vehicle_ent) {
        return __gm->vehicles.GetObjectByEntity(vehicle_ent);
    }

    return nullptr;
}

bool Player::PutToVehicle(Vehicle *vehicle, int seatID)
{
    if (vehicle == nullptr)
        return false;

    return __gm->mod->vtable.player_put_to_vehicle(this->entity, vehicle->GetEntity(), seatID);
}

mafia_player* Player::GetPedestrian()
{
    return (mafia_player*)entity->user_data;
}

void Player::ShowOnMap(bool visibility)
{
    __gm->mod->vtable.player_show_on_map(this->entity, visibility);
}

bool Player::GetMapVisibility()
{
    auto playerData = GetPedestrian();
    return playerData->is_visible_on_map;
}


//
// Vehicle
//

Vehicle::Vehicle(librg_entity *entity) : GameObject()
{
    this->entity = entity;
}

Vehicle::~Vehicle()
{
    if (!this->IsBeingRemoved() && !destroyedByGame)
        __gm->mod->vtable.vehicle_destroy(this->entity);
}

void Vehicle::ShowOnRadar(bool visibility)
{
    __gm->mod->vtable.vehicle_show_on_radar(this->entity, visibility);
}

bool Vehicle::GetRadarVisibility()
{
    auto vehicle = GetVehicle();
    return vehicle->is_car_in_radar;
}

void Vehicle::ShowOnMap(bool visibility)
{
    __gm->mod->vtable.vehicle_show_on_map(this->entity, visibility);
}

bool Vehicle::GetMapVisibility()
{
    auto vehicle = GetVehicle();
    return vehicle->is_visible_on_map;
}

int Vehicle::GetPlayerSeatID(Player *player) 
{
    if (!player) return -1;

    return __gm->mod->vtable.vehicle_get_player_seat_id(this->entity, player->GetEntity());
}

bool Vehicle::IsBeingStreamed()
{
    auto entity = GetEntity();
    return entity->control_peer != nullptr;
}

void Vehicle::SetPosition(zpl_vec3 pos)
{
    __gm->mod->vtable.vehicle_set_position(this->entity, pos);
}

zpl_vec3 Vehicle::GetPosition()
{
    return entity->position;
}

void Vehicle::SetDirection(zpl_vec3 dir)
{
    __gm->mod->vtable.vehicle_set_direction(this->entity, dir);
}

zpl_vec3 Vehicle::GetDirection()
{
    auto vehicle = (mafia_vehicle*)entity->user_data;
    if (vehicle) {
        return vehicle->rot_forward;
    } 

    return  { 0.0f, 0.0f, 0.0f };
}

void Vehicle::SetTransparency(float transparency)
{
    __gm->mod->vtable.vehicle_set_transparency(this->entity, transparency);
}

float Vehicle::GetTransparency()
{
    return GetVehicle()->transparency;
}

void Vehicle::SetCollisionState(bool state)
{
    __gm->mod->vtable.vehicle_set_collision_state(this->entity, state);
}

bool Vehicle::GetCollisionState()
{
    return GetVehicle()->collision_state;
}

void Vehicle::Repair()
{
    __gm->mod->vtable.vehicle_repair(this->entity);
}


void Vehicle::SetHeadingRotation(float angle)
{
    auto dir = ComputeDirVector(angle);
    SetDirection(dir);
}

float Vehicle::GetHeadingRotation()
{
    auto vehicle = GetVehicle();
    auto angle = DirToRotation180(vehicle->rot_forward);
    return angle;
}

mafia_vehicle *Vehicle::GetVehicle()
{
    return (mafia_vehicle *)entity->user_data;
}

void Vehicle::Destroy()
{
    __gm->mod->vtable.vehicle_destroy(GetEntity());
}
