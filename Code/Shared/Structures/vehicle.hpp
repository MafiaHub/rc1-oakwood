struct mafia_vehicle {
	mafia_vehicle() {
		zpl_zero_item(this);

		for (int i = 0; i < 4; i++)
			seats[i] = -1;
	}
#ifdef MAFIA_SDK_IMPLEMENTATION
    MafiaSDK::C_Car* car = nullptr;
	f32 inter_delta;
	/*zpl_vec3 target_pos;
	zpl_vec3 target_rot;
	zpl_vec3 target_rot_second;

	zpl_vec3 last_pos;
	zpl_vec3 last_rot;
	zpl_vec3 last_rot_second;
	
	zpl_vec3 interpolated_pos;
	zpl_vec3 interpolated_rot;
	zpl_vec3 interpolated_rot_second;*/

	/* interpolation table */
	struct {
		struct {
			zpl_vec3 start;
			zpl_vec3 target;
			zpl_vec3 error;
			f32  lastAlpha;
			f64  startTime;
			f64  finishTime;
		} pos;

		struct rot_data {
			zpl_vec3 start;
			zpl_vec3 target;
			zpl_vec3 error;
			f32  lastAlpha;
			f64  startTime;
			f64  finishTime;
		} rot, rot_second;

		u32 forceLocalZCounter;
	} interp;
#endif
	char model[32];
	i32 seats[4];
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