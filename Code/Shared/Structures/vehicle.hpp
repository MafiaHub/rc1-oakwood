struct mafia_vehicle {
#ifdef MAFIA_SDK_IMPLEMENTATION
    MafiaSDK::C_Car* car = nullptr;
	f32 inter_delta;
	interpolate3_hermite_t inter_pos;
	interpolate3_hermite_t inter_rot;
	interpolate3_hermite_t inter_speed;
#endif
	char model[32];
    i32 seats[4] = {0};
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
    zpl_vec3 rotation;
	zpl_vec3 speed;
};