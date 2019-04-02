#define OAKWOOD_DEBUG

librg_ctx network_context = { 0 };

f64 last_delta  = 0.0f;
f64 last_update = 0.0f;
bool hit_hook_skip = true;

struct _GlobalConfig {
    char server_address[32];
    char username[32];
    float view_distance;
    std::string localpath;
    std::string gamepath;
    // NOTE(DavoSK): Better result with high delay values for vehicle interpolation
    // Possible cuz of overinterpolation protection
    // TODO(DavoSK): Find real good value
    float interp_time_vehicle = 4.0f;
    float interp_time_player = 0.08f;
} GlobalConfig;

struct data {
    data() :
        dead(false) {
    }

    u32 entity_id;
    zpl_vec3 pose;
    bool dead;
} local_player;

struct shoot_info {
    S_vector pos;
    S_vector dir;
    S_vector screen_coord;
    DWORD player_base;
} local_shoot_data;
std::unordered_map<void*, shoot_info> shoot_queue;

IDirect3DDevice9* global_device = nullptr;
