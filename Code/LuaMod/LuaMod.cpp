/*

LuaMod is an example of lua bindings for Oakwood

*/

#include <Oakwood/Oakwood.hpp>
#include <sol.hpp>

#define BIND_FUNCTION(parent, name)\
	#name, &ZPL_JOIN3(parent,::,name)


sol::state lua;

OAK_MOD_MAIN{
	mod->name = "freeride";
	mod->author = "Oak Devs";
	mod->version = "v1.0.0";

	lua.open_libraries(sol::lib::base);

	lua.new_usertype<zpl_vec3>("zpl_vec3",
		BIND_FUNCTION(zpl_vec3, x),
		BIND_FUNCTION(zpl_vec3, y),
		BIND_FUNCTION(zpl_vec3, z)
	);

	lua.new_usertype<inventory_item>("inventory_item",
		BIND_FUNCTION(inventory_item, weaponId),
		BIND_FUNCTION(inventory_item, ammoLoaded),
		BIND_FUNCTION(inventory_item, ammoHidden),
		BIND_FUNCTION(inventory_item, _unk)
	);

	lua.new_usertype<Player>("Player",
		BIND_FUNCTION(Player, Spawn),
		BIND_FUNCTION(Player, Respawn),
		BIND_FUNCTION(Player, SetModel),
		BIND_FUNCTION(Player, GetModel),
		BIND_FUNCTION(Player, GetName),
		BIND_FUNCTION(Player, SetPosition),
		BIND_FUNCTION(Player, GetPosition),
		BIND_FUNCTION(Player, AddItem),
		BIND_FUNCTION(Player, ClearInventory),
		BIND_FUNCTION(Player, GetCurrentWeapon),
		BIND_FUNCTION(Player, Fadeout),
		BIND_FUNCTION(Player, SetHealth),
		BIND_FUNCTION(Player, GetHealth)
	);

	lua.new_usertype<GameMode>("GameMode",
		BIND_FUNCTION(GameMode, BroadcastMessage),
		BIND_FUNCTION(GameMode, SpawnWeaponDrop),
		BIND_FUNCTION(GameMode, SetOnPlayerConnected),
		BIND_FUNCTION(GameMode, SetOnPlayerDisconnected),
		BIND_FUNCTION(GameMode, SetOnPlayerDied),
		BIND_FUNCTION(GameMode, SetOnServerTick),
		BIND_FUNCTION(GameMode, SpawnWeaponDrop),
		BIND_FUNCTION(GameMode, SetOnPlayerChat)
	);

	lua["gm"] = new GameMode(mod);
	lua.script_file("main.lua");
}