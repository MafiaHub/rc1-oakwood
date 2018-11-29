#pragma once

/*
* Describes character controlled by player.
*/

class GameMode;
class Vehicle;

class Player : public GameObject {
public:
    friend class GameMode;
    Player(librg_entity *entity, mafia_player *ped);
    ~Player();

    //
    // Natives
    //

    void Spawn();
    void Respawn();
    
    void SetModel(std::string name);
    void SetModelByID(int modelID);
    std::string GetModel();

    std::string GetName();

    void SetPosition(zpl_vec3 position);
    zpl_vec3 GetPosition();

    void SetRotation(float rotation);
    float GetRotation();

    void AddItem(inventory_item *item);
    void ClearInventory();
    u32 GetCurrentWeapon();

    void Fadeout(bool fadeout, u32 duration, u32 color);
    void SetCamera(zpl_vec3 pos, zpl_vec3 rot);
    void UnlockCamera();

    void PlayAnimation(std::string animation);

    void SetHealth(f32 health); // input value gets multiplied by 2
    f32  GetHealth();           // output value gets divided by 2

    Vehicle *GetVehicle();

    //
    // Utilities
    //

    void SetPed(mafia_player *ped);

private:
    mafia_player *ped;
};
