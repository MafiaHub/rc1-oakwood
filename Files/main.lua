function new_vec3(x, y, z)
	local new_vec = zpl_vec3.new()
	new_vec.x = x
	new_vec.y = y
	new_vec.z = z
	return new_vec
end

function new_item(weaponId, ammoLoaded, ammoHidden)
	local item = inventory_item.new()
	item.weaponId = weaponId
	item.ammoLoaded = ammoLoaded
	item.ammoHidden = ammoHidden
	item._unk = 0
	return item
end

local weaponList = {
	{
		name = "Colt",
		model = "2c1911.i3d",
		item = new_item(9, 50, 50)
	},
	{
		name = "Grenade",
		model = "2grenade.i3d",
		item = new_item(15, 1, 1)
	}
}

function addWeapons(player)
	for idx,_ in pairs(weaponList) do
		player:AddItem(weaponList[idx].item)
	end
end

function getWeaponByID(id) 
	for idx,_ in pairs(weaponList) do
		if weaponList[idx].item.weaponId == id then
			return weaponList[idx]
		end
	end
	return nil
end

function generateSpawn()
	return new_vec3(-1984.88, -5.032, 23.14)
end

gm:SetOnServerTick(function()
	
end)

gm:SetOnPlayerConnected(function(player) 
	player:SetPosition(generateSpawn())
	addWeapons(player)
	player:Spawn()
	
	gm:BroadcastMessage("Player " .. player:GetName() .. " joined the server.", 0xFFFFFF);
end)

gm:SetOnPlayerDisconnected(function(player) 
	gm:BroadcastMessage("Player " .. player:GetName() .. " left the server.", 0xFFFFFF);
end)

gm:SetOnPlayerChat(function(player, msg)

	return true
end)

gm:SetOnPlayerDied(function(player) 
	player:Fadeout(true, 10, 0xFFFFFF)
	player:Fadeout(false, 1000, 0xFFFFFF)
	
	local weapon = getWeaponByID(player:GetCurrentWeapon())
	if weapon ~= nil then
		gm:SpawnWeaponDrop(player:GetPosition(), weapon.model, weapon.item)
	end
	
	player:ClearInventory()
	player:SetPosition(generateSpawn())
	player:SetHealth(100.0)
	addWeapons(player)
	player:Respawn()
	gm:BroadcastMessage("Player " .. player:GetName() .. " has died.");
end)