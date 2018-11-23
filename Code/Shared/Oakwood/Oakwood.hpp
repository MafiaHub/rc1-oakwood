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
	void SendMessageToPlayer(std::string text, Player *receiver, u32 color = 0xFFFFFF);
    void ChatPrint(std::string text);
    void SpawnWeaponDrop(zpl_vec3 position, std::string model, inventory_item item);
    Vehicle* SpawnVehicle(zpl_vec3 pos, float angle, const std::string& model);
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

private:
    std::function<void(Player*)> onPlayerConnected;
    std::function<void(Player*)> onPlayerDisconnected;
    std::function<void(Player*)> onPlayerDied;
    std::function<void(Player*,Player*,float)> onPlayerHit;
    std::function<bool(Player*,std::string msg)> onPlayerChat;
    std::function<void()> onServerTick;
    std::vector<Player*> players;

    Player* GetPlayerByEntity(librg_entity *entity);
};

static zpl_vec3 ComputeDirVector(float angle) {
	zpl_vec3 dir = { 0 };
	dir.x = (float)(::sin(zpl_to_radians(angle)));
	dir.z = (float)(::cos(zpl_to_radians(angle)));
	return dir;
}

static float DirToRotation180(zpl_vec3 dir) {
	return zpl_to_degrees(::atan2f(dir.x, dir.z));
}

static float DirToRotation360(zpl_vec3 dir) {
	auto val = zpl_to_degrees(::atan2f(dir.x, dir.z));

	if (val < 0) {
		val += 360.0f;
	}

	return val;
}