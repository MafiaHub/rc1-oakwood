#pragma once
typedef bool(__thiscall* C_Human_PoseSetPose_t)(MafiaSDK::C_Human* _this, S_vector pose);
typedef bool(__thiscall* C_Human_Do_Shoot_t)(void* _this, BOOL do_shoot, S_vector* pos);

auto spawn(zpl_vec3 position, 
                  zpl_vec3 rotation,
                  player_inventory inventory,
                  char *model,
                  char *name,
                  u32 current_wep,
                  f32 health,
                  bool is_local_player, 
                  int expectedWeaponId,
                  bool is_in_car) -> MafiaSDK::C_Player *;
auto despawn(MafiaSDK::C_Human* player) -> void;
auto giveWeapon(MafiaSDK::C_Human* player, int weapID, int ammo1, int ammo2) -> void;
auto removeWeapon(MafiaSDK::C_Human* player, short weapID) -> void;
inline auto get_local_entity();
inline auto get_local_player() -> mafia_player*;
inline auto get_local_ped() -> MafiaSDK::C_Player*;
inline auto get_player_from_base(void* base) -> librg_entity*;
inline auto get_vehicle_from_base(void* base) -> librg_entity*;
inline auto on_key_pressed(bool down, unsigned long key) -> void;

inline auto died(MafiaSDK::C_Actor* killer, int death_reason, DWORD hit_type, unsigned int player_part) -> void;
inline auto hit(
    MafiaSDK::C_Human* victim,
    DWORD hit_type, 
    const S_vector* unk1, 
    const S_vector* unk2, 
    const S_vector* unk3,
    float damage,
    MafiaSDK::C_Actor* attacker, 
    unsigned int player_part) -> void;
inline auto shoot(S_vector data) -> void;
inline auto weapondrop(inventory_item* item, char* model) -> void;
inline auto weaponchange(u32 index) -> void;
inline auto fromcar() -> void;
//TODO send inventory on each message related with weapons !
inline auto reload() -> void;
inline auto holster() -> void;
inline auto weaponpickup(librg_entity* item_entity) -> void;
inline auto throwgrenade(const S_vector & pos) -> void;
inline auto useactor(DWORD actor, int action, int seat_id, int unk3) -> void;
inline void use_door(MafiaSDK::C_Door* door, MafiaSDK::C_Door_Enum::States state);
inline auto hijack(DWORD car, int seat) -> void;
inline auto remove_temporary_actor(void* base) -> void;
inline auto car_destruct(void* base) -> void;
