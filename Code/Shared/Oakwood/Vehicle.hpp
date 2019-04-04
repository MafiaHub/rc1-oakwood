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

    void ShowOnMap(bool visibility);
    bool GetMapVisibility();

    int GetPlayerSeatID(Player *player);
    bool IsBeingStreamed();
    
    void SetPosition(zpl_vec3 pos);
    zpl_vec3 GetPosition();
    
    void SetDirection(zpl_vec3 dir);
    zpl_vec3 GetDirection();

    void SetTransparency(float transparency);
    float GetTransparency();

    void SetCollisionState(bool state);
    bool GetCollisionState();

    void Repair();

    void SetHeadingRotation(float angle);
    float GetHeadingRotation();

    mafia_vehicle *GetVehicle();
    void Destroy();

protected:
    bool destroyedByGame;
};