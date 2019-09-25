librg_ctx network_context = {0};
f64 last_delta = 0.0f;
f64 last_update = 0.0f;
bool hit_hook_skip = true;
bool use_actor_skip = false;
bool hijack_skip = false;

enum
{
    Menu_Pause,
    Menu_Connecting,
    Menu_MainMenu,
    Menu_Chat,
    Menu_DebugMode,
};

int menuActiveState = Menu_Chat;

enum ClientStates
{
    ClientState_Browser,
    ClientState_Infobox,
    ClientState_PasswordPrompt,
    ClientState_Connecting,
    ClientState_Connected,
};

int clientActiveState = ClientState_Browser;

void switchClientState(int state);

namespace ServerInfo
{
struct ServerData;
}

namespace modules::infobox
{
void displayError(const std::string &, std::function<void()> _cb = nullptr);
}

namespace modules::passwordPrompt
{
void init(ServerInfo::ServerData data);
}

struct _GlobalConfig
{
    char server_address[32];
    int port = 27010;
    char username[32];
    char password[32];
    bool passworded;
    bool reconnecting;
    float view_distance;
    std::string localpath;
    std::string gamepath;
    // NOTE(DavoSK): Better result with high delay values for vehicle interpolation
    // Possible cuz of overinterpolation protection
    // TODO(DavoSK): Find real good value
    float interp_time_vehicle = 4.0f;
    float interp_time_player = 0.08f;
} GlobalConfig;

struct data
{
    data() : dead(false), spec_id(-1), last_spec_id(-1)
    {
    }

    u32 entity_id;
    zpl_vec3 pose;
    bool dead;

    u8 is_shooting;
    zpl_vec3 aim_vector;

    // Spectator
    i32 spec_id;
    i32 last_spec_id;
} local_player;

/*struct shoot_info
{
    S_vector pos;
    S_vector dir;
    S_vector screen_coord;
    DWORD player_base;
} local_shoot_data;
std::unordered_map<void *, shoot_info> shoot_queue;*/

std::vector<MafiaSDK::C_Car *> car_delte_queue;
IDirect3DDevice9 *global_device = nullptr;

#ifdef OAK_FEATURE_VEHICLE_CACHE
std::unordered_map<u32, MafiaSDK::C_Car *> car_cache;
#endif
