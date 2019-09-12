#define OAK_VEHICLE_MODEL_SIZE 32

struct mafia_vehicle_tyre {
	u32 flags;
	f32 health;
};

struct mafia_vehicle_deform {
    u32 mesh_index;
    u32 vertex_index;
    zpl_vec3 normal;
    zpl_vec3 position;
};

enum {
    CLIENTSIDE_VEHICLE_STREAMER_REMOVED = (1 << 10)
};

struct mafia_vehicle : public oak_object {
    mafia_vehicle() {
        reset();
    }

	void reset() {
		for (int i = 0; i < 4; i++)
			seats[i] = -1;

		for (int i = 0; i < 4; i++) {
			tyres[i].health = 100.0f;
			tyres[i].flags = 0x0;
		}

		transparency = 1.0f;
		collision_state = true;
        lock = false;
	}

	char model[OAK_VEHICLE_MODEL_SIZE];
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
	u8 is_car_in_radar;
	u8 is_visible_on_map;
    zpl_vec3 rot_forward;
	zpl_vec3 rot_right;
    zpl_vec3 rot_up;
    zpl_vec3 rot_speed;
	zpl_vec3 speed;
	f32 transparency;
	b32 collision_state;
    b32 lock;
	mafia_vehicle_tyre tyres[4];
	u8 destroyed_components[15];
    std::vector<mafia_vehicle_deform> deform_deltas;

	#ifdef MAFIA_SDK_IMPLEMENTATION
    MafiaSDK::C_Car* car = nullptr;
	b32 wants_explode;
    u64 clientside_flags;
	/* interpolation table */
	struct {
        lib_inter* pos, *rot, *rot_up;
		u32 force_localz_counter;
	} interp;
#endif
};
