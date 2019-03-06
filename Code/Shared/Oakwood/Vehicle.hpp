#pragma once

/* 
* Vehicle 
*/

class GameMode;
class Player;

class Vehicle : public GameObject {
public:
    friend class GameMode;
    Vehicle(librg_entity *entity);
    virtual ~Vehicle() override;

    //
    // Natives
    //

    void ShowOnRadar(bool visibility);
    bool GetRadarVisibility();

    int GetPlayerSeatID(Player *player);
    
    void SetPosition(zpl_vec3 pos);
    zpl_vec3 GetPosition();
    
    void SetDirection(zpl_vec3 dir);
    zpl_vec3 GetDirection();

    void SetHeadingRotation(float angle);
    float GetHeadingRotation();

    mafia_vehicle *GetVehicle();
};