#define OAK_INTERNAL
#include "Oakwood.hpp"

GameMode::GameMode(oak_api *mod) {
    this->mod = mod;

    mod->on_player_connected = [&](librg_event_t* evnt, librg_entity_t* entity, mafia_player* ped) {
        Player player = Player();

        player.SetModel("Tommy.i3d");
        player.SetHealth(100);

        if (onPlayerConnected)
            onPlayerConnected(&player);

        players.push_back(player);
    };
}

GameMode::~GameMode() {
    this->mod = nullptr;
}

void GameMode::BroadcastMessage(std::string text, u32 color)
{
    mod->vtable.broadcast_msg_color(text.c_str(), color);
}

void GameMode::SetOnPlayerConnected(std::function<void(Player*)> callback)
{
    onPlayerConnected = callback;
}

Player::Player()
{
}

Player::~Player()
{
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

void Player::SetHealth(f32 health)
{
    ped->health = health * 2.0f;
}

f32 Player::GetHealth()
{
    return ped->health / 2.0f;
}
