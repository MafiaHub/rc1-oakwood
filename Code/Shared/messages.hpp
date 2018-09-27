enum {
    ENTITY_INTERPOLATED = 1 << 10,
};

enum {
	TYPE_PLAYER,
	TYPE_WEAPONDROP
};

enum {
	NETWORK_OAKWOOD_ENTRY = LIBRG_EVENT_LAST + 250, 
	NETWORK_PLAYER_SPAWN,
	NETWORK_PLAYER_SHOOT,
	NETWORK_PLAYER_THROW_GRENADE,
	NETWORK_PLAYER_WEAPON_CHANGE,
	NETWORK_PLAYER_WEAPON_REMOVE,
	NETWORK_PLAYER_WEAPON_ADD,
	NETWORK_PLAYER_WEAPON_DROP,
	NETWORK_PLAYER_WEAPON_PICKUP,
	NETWORK_PLAYER_HIT,
	NETWORK_PLAYER_DIE,
	NETWORK_PLAYER_RESPAWN,
	
	NETWORK_SEND_CONSOLE_MSG,
	NETWORK_SEND_CHAT_MSG

};