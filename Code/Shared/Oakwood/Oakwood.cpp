#define OAK_INTERNAL
#include "Oakwood.hpp"

// Listings
#include "VehicleListing.hpp"
#include "PlayerModelListing.hpp"

GameMode *__gm = nullptr;

GameMode::GameMode(oak_api *mod) {
    this->mod = mod;
    __gm = this;

    mod->on_player_connected = [=](librg_event* evnt, librg_entity* entity, mafia_player* ped) {
        
        Player *player = new Player(entity, ped);

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

        delete player;

        players.RemoveObject(player);
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

            vehicles.RemoveObject(vehicle);
        }
    };

    mod->on_player_chat = [=](librg_entity* entity, std::string msg) {
        auto player = players.GetObjectByEntity(entity);

        if (!player) {
            printf("[OAKWOOD] Unregistered entity sends message!");
            return false;
        }
        
        auto args = SplitStringByNewline(msg);

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
    auto vehicle = new Vehicle(entity, (mafia_vehicle*)entity->user_data);
    vehicles.AddObject(vehicle);

    return vehicle;
}

Vehicle *GameMode::SpawnVehicleByID(zpl_vec3 pos, float angle, int modelID)
{
    if (modelID < 0 || modelID >= zpl_count_of(VehicleCatalogue))
        return nullptr;

    auto modelName = std::string(VehicleCatalogue[modelID]) + ".i3d";

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

//
// Player
//

Player::Player(librg_entity *entity, mafia_player *ped)
{
    this->entity = entity;
    this->ped = ped;
}

Player::~Player()
{
}

void Player::Spawn()
{
    __gm->mod->vtable.player_spawn(entity);
}

void Player::Respawn()
{
    __gm->mod->vtable.player_respawn(entity);
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
    return std::string(ped->model);
}

std::string Player::GetName()
{
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
    for (size_t i = 0; i < 8; i++)
        ped->inventory.items[i] = { -1, 0, 0, 0 };
}

u32 Player::GetCurrentWeapon()
{
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
    __gm->mod->vtable.player_play_animation(entity, animation.c_str());
}

void Player::SetHealth(f32 health)
{
    __gm->mod->vtable.player_set_health(entity, health * 2.0f);
}

f32 Player::GetHealth()
{
    return ped->health / 2.0f;
}

Vehicle *Player::GetVehicle()
{
    auto vehicle_ent = __gm->mod->vtable.player_get_vehicle(this->entity);

    if (vehicle_ent) {
        return __gm->vehicles.GetObjectByEntity(vehicle_ent);
    }

    return nullptr;
}

void Player::SetPed(mafia_player *ped)
{
    this->ped = ped;
}

//
// Vehicle
//

Vehicle::Vehicle(librg_entity *entity, mafia_vehicle *vehicle)
{
    this->entity = entity;
    this->vehicle = vehicle;
}

Vehicle::~Vehicle()
{
}

void Vehicle::ShowOnRadar(bool visibility)
{
    __gm->mod->vtable.vehicle_show_on_radar(this->entity, visibility);
}

bool Vehicle::GetRadarVisibility()
{
    return this->vehicle->is_car_in_radar;
}

int Vehicle::GetPlayerSeatID(Player *player) 
{
    if (!player) return -1;

    return __gm->mod->vtable.vehicle_get_player_seat_id(this->entity, player->GetEntity());
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
        return vehicle->rotation;
    } 

    return  { 0.0f, 0.0f, 0.0f };
}

void Vehicle::SetHeadingRotation(float angle)
{
    auto dir = ComputeDirVector(angle);
    SetDirection(dir);
}

float Vehicle::GetHeadingRotation()
{
    auto angle = DirToRotation180(vehicle->rotation);
    return angle;
}
