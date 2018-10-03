#pragma once

/*
* Describes character controlled by player.
*/

class Player {
public:
    Player(librg_entity_t *entity, mafia_player *ped);
    ~Player();

    //
    // Natives
    //

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

    void Fadeout(bool fadeout, u32 duration, u32 color);

    void SetHealth(f32 health); // input value gets multiplied by 2
    f32  GetHealth();           // output value gets divided by 2

    //
    // Utilities
    //

    b32 CompareWith(librg_entity_t *entity);
    void SetPed(mafia_player *ped);

private:
    mafia_player *ped;
    librg_entity_t *entity;
};
