struct mafia_vehicle_tyre {
	u32 flags;
	f32 health;
};

struct mafia_vehicle {
	mafia_vehicle() {
		zpl_zero_item(this);

		for (int i = 0; i < 4; i++)
			seats[i] = -1;

		for (int i = 0; i < 4; i++) {
			tyres[i].health = 100.0f;
			tyres[i].flags = 0x1;
		}
	}
#ifdef MAFIA_SDK_IMPLEMENTATION
    MafiaSDK::C_Car* car = nullptr;
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
	float engine_rpm;
	float accelerating;
    zpl_vec3 rotation;
	zpl_vec3 rotation_second;
	zpl_vec3 speed;
	mafia_vehicle_tyre tyres[4];
};