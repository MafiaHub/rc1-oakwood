lua.new_usertype<GameMode>("GameMode",
	BIND_FUNCTION(GameMode,  BroadcastMessage),
	BIND_FUNCTION(GameMode,  SendMessageToPlayer),
	BIND_FUNCTION(GameMode,  ChatPrint),
	BIND_FUNCTION(GameMode,  SpawnWeaponDrop),
	BIND_FUNCTION(GameMode,  SpawnVehicle),
	BIND_FUNCTION(GameMode,  SpawnVehicleByID),
	BIND_FUNCTION(GameMode,  SetOnPlayerConnected),
	BIND_FUNCTION(GameMode,  SetOnPlayerDisconnected),
	BIND_FUNCTION(GameMode,  SetOnPlayerDied),
	BIND_FUNCTION(GameMode,  SetOnPlayerHit),
	BIND_FUNCTION(GameMode,  SetOnPlayerChat),
	BIND_FUNCTION(GameMode,  SetOnServerTick),
	BIND_FUNCTION(GameMode,  AddCommandHandler)
);

lua.new_usertype<Player>("Player",
	BIND_FUNCTION(Player,  Spawn),
	BIND_FUNCTION(Player,  Respawn),
	BIND_FUNCTION(Player,  SetModel),
	BIND_FUNCTION(Player,  SetModelByID),
	BIND_FUNCTION(Player,  GetModel),
	BIND_FUNCTION(Player,  GetName),
	BIND_FUNCTION(Player,  SetPosition),
	BIND_FUNCTION(Player,  GetPosition),
	BIND_FUNCTION(Player,  SetRotation),
	BIND_FUNCTION(Player,  GetRotation),
	BIND_FUNCTION(Player,  AddItem),
	BIND_FUNCTION(Player,  ClearInventory),
	BIND_FUNCTION(Player,  GetCurrentWeapon),
	BIND_FUNCTION(Player,  Fadeout),
	BIND_FUNCTION(Player,  SetCamera),
	BIND_FUNCTION(Player,  UnlockCamera),
	BIND_FUNCTION(Player,  PlayAnimation),
	BIND_FUNCTION(Player,  SetHealth),
	BIND_FUNCTION(Player,  CompareWith),
	BIND_FUNCTION(Player,  SetPed)
);

lua.new_usertype<Vehicle>("Vehicle",
	BIND_FUNCTION(Vehicle,  SetPosition),
	BIND_FUNCTION(Vehicle,  GetPosition),
	BIND_FUNCTION(Vehicle,  SetDirection),
	BIND_FUNCTION(Vehicle,  GetDirection),
	BIND_FUNCTION(Vehicle,  SetHeadingRotation),
	BIND_FUNCTION(Vehicle,  GetHeadingRotation)
);

