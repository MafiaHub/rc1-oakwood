#pragma once

/* 
* Vehicle 
*/

class GameMode;

class Vehicle {
public:
    friend class GameMode;
    Vehicle(librg_entity *entity, mafia_vehicle *vehicle);
    ~Vehicle();

    //
    // Natives
    //
    
    void SetPosition(zpl_vec3 pos);
    zpl_vec3 GetPosition();
    
    void SetDirection(zpl_vec3 dir);
    zpl_vec3 GetDirection();

    void SetHeadingRotation(float angle);
    float GetHeadingRotation();

protected:
    librg_entity *entity;

private:
    mafia_vehicle *vehicle;
};