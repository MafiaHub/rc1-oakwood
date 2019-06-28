#pragma once

/*
* Describes character controlled by player.
*/

class GameMode;
class Vehicle;

class Player : public GameObject {
public:
    friend class GameMode;
    Player(librg_entity *entity);
    ~Player();

    //
    // Natives
    //

    void Spawn(zpl_vec3 pos);

    void SetModel(std::string name);
    void SetModelByID(int modelID);
    std::string GetModel();

    void ShowOnMap(bool visibility);
    bool GetMapVisibility();

    void ShowNameplate(bool visibility);
    bool GetNameplateVisibility();

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
    void Die();

    Vehicle *GetVehicle();
    bool PutToVehicle(Vehicle *vehicle, int seatID);

    mafia_player *GetPedestrian();
};
