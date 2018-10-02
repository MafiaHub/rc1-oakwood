#pragma once

#include "mod_api.hpp"

class Player {
public:
    Player(librg_entity_t *entity, mafia_player *ped);
    ~Player();

    void Spawn();
    void Respawn();
    
    void SetModel(std::string name);
    std::string GetModel();

    std::string GetName();

    void SetPosition(zpl_vec3 position);
    zpl_vec3 GetPosition();

    void AddItem(inventory_item *item);
    void ClearInventory();
    u32 GetCurrentWeapon();

    void SetHealth(f32 health); // input value gets multiplied by 2
    f32  GetHealth();           // output value gets divided by 2

    b32 CompareWith(librg_entity_t *entity);

private:
    mafia_player *ped;
    librg_entity_t *entity;
};

class GameMode {
public:
    GameMode(oak_api *mod);
    ~GameMode();

    void BroadcastMessage(std::string text, u32 color = 0xFFFFFF);
    void SpawnWeaponDrop(zpl_vec3 position, std::string model, inventory_item item);

    void SetOnPlayerConnected(std::function<void(Player*)> callback);
    void SetOnPlayerDisconnected(std::function<void(Player*)> callback);
    void SetOnPlayerDied(std::function<void(Player*)> callback);
    void SetOnServerTick(std::function<void()> callback);

    oak_api *mod;

private:
    std::function<void(Player*)> onPlayerConnected;
    std::function<void(Player*)> onPlayerDisconnected;
    std::function<void(Player*)> onPlayerDied;
    std::function<void()> onServerTick;
    std::vector<Player*> players;

    Player* GetPlayerByEntity(librg_entity_t *entity);
};

