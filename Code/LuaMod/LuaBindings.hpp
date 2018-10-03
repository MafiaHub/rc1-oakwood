lua.new_usertype<GameMode>("GameMode",
	BIND_FUNCTION(GameMode,  BroadcastMessage),
	BIND_FUNCTION(GameMode,  ChatPrint),
	BIND_FUNCTION(GameMode,  SpawnWeaponDrop),
	BIND_FUNCTION(GameMode,  SetOnPlayerConnected),
	BIND_FUNCTION(GameMode,  SetOnPlayerDisconnected),
	BIND_FUNCTION(GameMode,  SetOnPlayerDied),
	BIND_FUNCTION(GameMode,  SetOnPlayerChat),
	BIND_FUNCTION(GameMode,  SetOnServerTick)
);

lua.new_usertype<Player>("Player",
	BIND_FUNCTION(Player,  Spawn),
	BIND_FUNCTION(Player,  Respawn),
	BIND_FUNCTION(Player,  SetModel),
	BIND_FUNCTION(Player,  GetModel),
	BIND_FUNCTION(Player,  GetName),
	BIND_FUNCTION(Player,  SetPosition),
	BIND_FUNCTION(Player,  GetPosition),
	BIND_FUNCTION(Player,  AddItem),
	BIND_FUNCTION(Player,  ClearInventory),
	BIND_FUNCTION(Player,  GetCurrentWeapon),
	BIND_FUNCTION(Player,  Fadeout),
	BIND_FUNCTION(Player,  SetHealth),
	BIND_FUNCTION(Player,  CompareWith)
);