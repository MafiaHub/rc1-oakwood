#define OAK_INTERNAL
#include "Oakwood.hpp"

GameMode *__gm = nullptr;

GameMode::GameMode(oak_api *mod) {
    this->mod = mod;
    __gm = this;

    mod->on_player_connected = [=](librg_event_t* evnt, librg_entity_t* entity, mafia_player* ped) {
        
        Player *player = new Player(entity, ped);

        player->SetModel("Tommy.i3d");
        player->SetHealth(100);

        if (onPlayerConnected)
            onPlayerConnected(player);

        players.push_back(player);
    };

    mod->on_player_disconnected = [=](librg_event_t* evnt, librg_entity_t* entity) {

        auto player = GetPlayerByEntity(entity);

        if (!player) {
            printf("[OAKWOOD] Unregistered entity was just disconnected !");
            return;
        }

        if (onPlayerDisconnected)
            onPlayerDisconnected(player);

        delete player;

        players.erase(std::remove(players.begin(), players.end(), player), players.end());
    };

    mod->on_player_died = [=](librg_entity_t* entity, mafia_player* ped) {

        auto player = GetPlayerByEntity(entity);

        if (!player) {
            printf("[OAKWOOD] Unregistered entity just died !");
            return;
        }

        if (onPlayerDied)
            onPlayerDied(player);
    };

    mod->on_player_chat = [=](librg_entity_t* entity, std::string msg) {
        auto player = GetPlayerByEntity(entity);

        if (!player) {
            printf("[OAKWOOD] Unregistered entity sends message !");
            return false;
        }

        bool is_handled = false;

        if (onPlayerChat) {
            is_handled = onPlayerChat(player, msg);
        }
        
        return is_handled;
    };
}

GameMode::~GameMode() {
    this->mod = nullptr;
}

void GameMode::BroadcastMessage(std::string text, u32 color)
{
    mod->vtable.broadcast_msg_color(text.c_str(), color);
}

void GameMode::ChatPrint(std::string text)
{
    mod->vtable.chat_print(text.c_str());
}

void GameMode::SpawnWeaponDrop(zpl_vec3 position, std::string model, inventory_item item)
{
    mod->vtable.drop_spawn(position, (char *)model.c_str(), item);
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

void GameMode::SetOnPlayerChat(std::function<bool(Player*, std::string msg)> callback)
{
    onPlayerChat = callback;
}

void GameMode::SetOnServerTick(std::function<void()> callback)
{
    onServerTick = callback;
}

Player * GameMode::GetPlayerByEntity(librg_entity_t * entity)
{
    for (auto player : players) {
        if (player->CompareWith(entity))
            return player;
    }

    return nullptr;
}

Player::Player(librg_entity_t *entity, mafia_player *ped)
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
    sprintf(ped->model, "%s", name.c_str());
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

void Player::SetRotation(zpl_vec3 rotation)
{
    __gm->mod->vtable.player_set_rotation(entity, rotation);
}

zpl_vec3 Player::GetRotation()
{
    auto player = (mafia_player*)(entity->user_data);
    if(player) {
        return  player->rotation;
    }

	return  {0.0f, 0.0f, 0.0f};
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
    ped->health = health * 2.0f;
}

f32 Player::GetHealth()
{
    return ped->health / 2.0f;
}

b32 Player::CompareWith(librg_entity_t * entity)
{
    return this->entity == entity;
}

void Player::SetPed(mafia_player *ped)
{
    this->ped = ped;
}