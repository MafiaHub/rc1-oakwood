struct mafia_vehicle {
#ifdef MAFIA_SDK_IMPLEMENTATION
    MafiaSDK::C_Car* car = nullptr;
	f32 inter_delta;
	zpl_vec3 target_pos;
	zpl_vec3 target_rot;
	zpl_vec3 target_rot_second;

	zpl_vec3 last_pos;
	zpl_vec3 last_rot;
	zpl_vec3 last_rot_second;
	
	zpl_vec3 interpolated_pos;
	zpl_vec3 interpolated_rot;
	zpl_vec3 interpolated_rot_second;
#endif
	char model[32];
    i32 seats[4] = {-1, -1, -1, -1};
    float engine_health;
    float health;
    u8 horn;
    u8 siren;
    u8 sound_enabled;
    float hand_break;
    float speed_limit;
    i32 gear;
    float break_val;
    float clutch;
    float wheel_angle;
    u8 engine_on;
    float fuel;
	float accelerating;
    zpl_vec3 rotation;
	zpl_vec3 rotation_second;
	zpl_vec3 speed;
};