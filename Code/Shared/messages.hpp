enum {
    NETWORK_OAKWOOD_ENTRY = LIBRG_EVENT_LAST + 250, 
    NETWORK_PLAYER_UPDATE_SCOREBOARD,
    NETWORK_PLAYER_UPDATE_GAMEMAP,
    NETWORK_PLAYER_SPAWN,
    NETWORK_PLAYER_SHOOT,
    NETWORK_PLAYER_USE_ACTOR,
    NETWORK_PLAYER_USE_DOORS,
    NETWORK_PLAYER_HIJACK,
    NETWORK_PLAYER_THROW_GRENADE,
    NETWORK_PLAYER_WEAPON_CHANGE,
    NETWORK_PLAYER_WEAPON_HOLSTER, 
    NETWORK_PLAYER_WEAPON_RELOAD,
    NETWORK_PLAYER_WEAPON_REMOVE,
    NETWORK_PLAYER_WEAPON_ADD,
    NETWORK_PLAYER_WEAPON_DROP,
    NETWORK_PLAYER_WEAPON_PICKUP,
    NETWORK_PLAYER_HIT,
    NETWORK_PLAYER_DIE,
    NETWORK_PLAYER_SET_POS,
    NETWORK_PLAYER_SET_ROT,
    NETWORK_PLAYER_SET_HEALTH,
    NETWORK_PLAYER_SET_MODEL,
    NETWORK_PLAYER_SET_CAMERA,
    NETWORK_PLAYER_UNLOCK_CAMERA,
    NETWORK_PLAYER_PLAY_ANIMATION,
    NETWORK_PLAYER_PUT_TO_VEHICLE,
    NETWORK_PLAYER_FROM_CAR,
    NETWORK_PLAYER_MAP_VISIBILITY,

    //Vehicle
    NETWORK_VEHICLE_WHEEL_DROPOUT,
    NETWORK_VEHICLE_COMPONENT_DROPOUT,
    NETWORK_VEHICLE_WHEEL_UPDATE,
    NETWORK_VEHICLE_EXPLODE,
    NETWORK_VEHICLE_DEFORM_DELTA,
    NETWORK_VEHICLE_RADAR_VISIBILITY,
    NETWORK_VEHICLE_MAP_VISIBILITY,
    NETWORK_VEHICLE_SET_POS,
    NETWORK_VEHICLE_SET_DIR,
    NETWORK_VEHICLE_GAME_DESTROY,
    NETWORK_VEHICLE_SET_TRANSPARENCY,
    NETWORK_VEHICLE_SET_COLLISION_STATE,
    NETWORK_VEHICLE_REPAIR,

    //Other
    NETWORK_SEND_CONSOLE_MSG,
    NETWORK_SEND_CHAT_MSG,
    NETWORK_SEND_FADEOUT,
    NETWORK_SEND_REJECTION,
    
    //VoIP
    NETWORK_SEND_VOIP_DATA,

    //WIP
    NETWORK_NPC_CREATE
};
