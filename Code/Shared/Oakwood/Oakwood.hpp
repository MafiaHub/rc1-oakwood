#pragma once

#include "mod_api.hpp"

#include "Player.hpp"
#include "Vehicle.hpp"

/*
* Handles initialization and player management.
*/

#include <unordered_map>

using ArgumentList = std::vector<std::string>;

class GameMode {
public:
    GameMode(oak_api *mod);
    ~GameMode();

    //
    // Natives
    //

    void BroadcastMessage(std::string text, u32 color = 0xFFFFFF);
    void SendMessageToPlayer(std::string text, Player *receiver, u32 color = 0xFFFFFF);
    void ChatPrint(std::string text);
    void SpawnWeaponDrop(zpl_vec3 position, std::string model, inventory_item item);
    Vehicle* SpawnVehicle(zpl_vec3 pos, float angle, const std::string& model, b32 show_in_radar = true);
    Vehicle* SpawnVehicleByID(zpl_vec3 pos, float angle, int modelID);

    //
    // Event handling
    //

    void SetOnPlayerConnected(std::function<void(Player*)> callback);
    void SetOnPlayerDisconnected(std::function<void(Player*)> callback);
    void SetOnPlayerDied(std::function<void(Player*)> callback);
    void SetOnPlayerHit(std::function<void(Player*,Player*,float)> callback);
    void SetOnPlayerChat(std::function<bool(Player*,std::string msg)> callback);
    void SetOnServerTick(std::function<void()> callback);

    //
    // Global stuff
    //

    oak_api *mod;

    //
    // Command handler
    //

    void AddCommandHandler(std::string command, std::function<bool(Player*,ArgumentList)> callback);

private:
    std::function<void(Player*)> onPlayerConnected;
    std::function<void(Player*)> onPlayerDisconnected;
    std::function<void(Player*)> onPlayerDied;
    std::function<void(Player*,Player*,float)> onPlayerHit;
    std::function<bool(Player*,std::string msg)> onPlayerChat;
    std::function<void()> onServerTick;
    std::vector<Player*> players;
    std::unordered_map<std::string, std::function<bool(Player*,ArgumentList)>> commands;

    Player* GetPlayerByEntity(librg_entity *entity);
};

#include <sstream>
#include <iterator>

static std::vector<std::string> SplitStringByNewline(const std::string& subject)
{
    std::istringstream ss{ subject };
    using StrIt = std::istream_iterator<std::string>;
    std::vector<std::string> container{ StrIt{ss}, StrIt{} };
    return container;
}

