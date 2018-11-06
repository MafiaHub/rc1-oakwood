#pragma once

/* 
* Vehicle 
*/
class Vehicle {
public:
    Vehicle(librg_entity *entity, mafia_vehicle *vehicle);
    ~Vehicle();

    //
    // Natives
    //
    
	void SetPos(zpl_vec3 pos);
	zpl_vec3 GetPos();
	
	void SetDir(zpl_vec3 dir);
	zpl_vec3 GetDir();
private:
    mafia_vehicle *vehicle;
    librg_entity *entity;
};