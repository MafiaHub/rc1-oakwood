#pragma once

#include "mod_api.hpp"

#include "Player.hpp"
#include "Vehicle.hpp"

/*
* Handles initialization and player management.
*/

class GameMode {
public:
    GameMode(oak_api *mod);
    ~GameMode();

    //
    // Natives
    //

    void BroadcastMessage(std::string text, u32 color = 0xFFFFFF);
    void ChatPrint(std::string text);
    void SpawnWeaponDrop(zpl_vec3 position, std::string model, inventory_item item);
    Vehicle* SpawnVehicle(zpl_vec3 pos, zpl_vec3 rot, const std::string& model);

    //
    // Event handling
    //

    void SetOnPlayerConnected(std::function<void(Player*)> callback);
    void SetOnPlayerDisconnected(std::function<void(Player*)> callback);
    void SetOnPlayerDied(std::function<void(Player*)> callback);
    void SetOnPlayerChat(std::function<bool(Player*,std::string msg)> callback);
    void SetOnServerTick(std::function<void()> callback);

    //
    // Global stuff
    //

    oak_api *mod;

private:
    std::function<void(Player*)> onPlayerConnected;
    std::function<void(Player*)> onPlayerDisconnected;
    std::function<void(Player*)> onPlayerDied;
    std::function<bool(Player*,std::string msg)> onPlayerChat;
    std::function<void()> onServerTick;
    std::vector<Player*> players;

    Player* GetPlayerByEntity(librg_entity *entity);
};

