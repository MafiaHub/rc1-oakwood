#pragma once

/* 
    This framework offers end-user features for server developers to easily design their
    game modes for their server.

    Oakwood Framework encapsulates the server's low-level modding API and helps you micro-manage some server aspects, it encapsulates game objects to
    easily understandable classes that define the behavior on a server. It also brings various features built on top, that
    accelerate the game mode development, such as command handling, game object management and a couple of utility helpers.

    This module should preferably be used when writing language bindings, as it's the most complete suite of tools yet to date.
    You can, however, use this framework in a native C++ environment if you're comfortable with that.
*/

#include "mod_api.hpp"

class GameObject
{
public:
    GameObject() { removed = false; }
    virtual ~GameObject() {}
    b32 CompareWith(librg_entity *entity);
    inline librg_entity *GetEntity() { return entity; }
    b32 IsBeingRemoved() { return removed; }
    void FlagForRemoval() { removed = true; }

protected:
    librg_entity *entity;

private:
    b32 removed;
};

#include "Player.hpp"
#include "Vehicle.hpp"

/*
* Handles initialization and player management.
*/

#include <unordered_map>

using ArgumentList = std::vector<std::string>;

template<typename T>
class ObjectManager
{
public:
    inline T* GetObjectByEntity(librg_entity *entity)
    {
        for (GameObject *object : objects) {
            if (object && object->CompareWith(entity))
                return (T*)object;
        }

        return nullptr;
    }

    inline T *GetObjectByID(int idx) {
        if (idx < 0 || idx >= objects.size()) {
            return nullptr;
        }

        return objects.at(idx);
    }

    inline void AddObject(T *object) {
        objects.push_back(object);
    }

    inline void RemoveObject(T *object) {
        object->FlagForRemoval();
        objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
    }

    inline size_t GetNumberOfObjects() {
        return objects.size();
    }

private:
    std::vector<T*> objects;
};

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
    void SetOnVehicleDestroyed(std::function<void(Vehicle *)> callback);

    //
    // Global stuff
    //

    oak_api *mod;

    //
    // Command handler
    //

    void AddCommandHandler(std::string command, std::function<bool(Player*,ArgumentList)> callback);
    
    //
    // Helpers
    //
    
    std::string ImplodeArgumentList(ArgumentList args);

    //
    // Object pools
    //

    ObjectManager<Player> players;
    ObjectManager<Vehicle> vehicles;

private:
    std::function<void(Player*)> onPlayerConnected;
    std::function<void(Player*)> onPlayerDisconnected;
    std::function<void(Player*)> onPlayerDied;
    std::function<void(Player*,Player*,float)> onPlayerHit;
    std::function<bool(Player*,std::string msg)> onPlayerChat;
    std::function<void(Vehicle*)> onVehicleDestroyed;
    std::function<void()> onServerTick;
    std::unordered_map<std::string, std::function<bool(Player*,ArgumentList)>> commands;
};
