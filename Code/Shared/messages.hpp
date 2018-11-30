enum {
    ENTITY_INTERPOLATED = 1 << 10,
};

enum {
    TYPE_PLAYER,
    TYPE_VEHICLE,
    TYPE_WEAPONDROP
};

enum {
    NETWORK_OAKWOOD_ENTRY = LIBRG_EVENT_LAST + 250, 
    NETWORK_PLAYER_SPAWN,
    NETWORK_PLAYER_SHOOT,
    NETWORK_PLAYER_USE_ACTOR,
    NETWORK_PLAYER_HIJACK,
    NETWORK_PLAYER_THROW_GRENADE,
    NETWORK_PLAYER_WEAPON_CHANGE,
    NETWORK_PLAYER_WEAPON_HOLSTER, 
    NETWORK_PLAYER_WEAPON_RELOAD,
    NETWORK_PLAYER_WEAPON_REMOVE,
    NETWORK_PLAYER_WEAPON_ADD,
    NETWORK_PLAYER_WEAPON_DROP,
    NETWORK_PLAYER_WEAPON_PICKUP,
    NETWORK_PLAYER_INVENTORY_SYNC,
    NETWORK_PLAYER_HIT,
    NETWORK_PLAYER_DIE,
    NETWORK_PLAYER_RESPAWN,
    NETWORK_PLAYER_SET_POS,
    NETWORK_PLAYER_SET_ROT,
    NETWORK_PLAYER_SET_HEALTH,
    NETWORK_PLAYER_SET_MODEL,
    NETWORK_PLAYER_SET_CAMERA,
    NETWORK_PLAYER_UNLOCK_CAMERA,
    NETWORK_PLAYER_PLAY_ANIMATION,
    NETWORK_PLAYER_PUT_TO_VEHICLE,

    //Vehicle
    NETWORK_VEHICLE_WHEEL_DROPOUT,
    NETWORK_VEHICLE_COMPONENT_DROPOUT,
    NETWORK_VEHICLE_WHEEL_UPDATE,
    NETWORK_VEHICLE_EXPLODE,
    NETWORK_VEHICLE_DEFORM_DELTA,
    NETWORK_VEHICLE_RADAR_VISIBILITY,
    NETWORK_VEHICLE_SET_POS,
    NETWORK_VEHICLE_SET_DIR,

    //Other
    NETWORK_SEND_CONSOLE_MSG,
    NETWORK_SEND_CHAT_MSG,
    NETWORK_SEND_FADEOUT,
    
    //VoIP
    NETWORK_SEND_VOIP_DATA,

    //WIP
    NETWORK_NPC_CREATE
};
