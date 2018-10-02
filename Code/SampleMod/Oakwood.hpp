#pragma once

#include "mod_api.hpp"

class Player {
public:
    Player();
    ~Player();

    void Spawn();
    void Respawn();
    
    void SetModel(std::string name);
    std::string GetModel();

    std::string GetName();

    void SetHealth(f32 health); // input value gets multiplied by 2
    f32  GetHealth();           // output value gets divided by 2

private:
    mafia_player *ped;
    librg_entity_t *entity;
};

class GameMode {
public:
    GameMode(oak_api *mod);
    ~GameMode();

    void BroadcastMessage(std::string text, u32 color = 0xFFFFFF);

    void SetOnPlayerConnected(std::function<void(Player*)> callback);

private:
    oak_api *mod;

    std::function<void(Player*)> onPlayerConnected;
    std::vector<Player> players;
};

