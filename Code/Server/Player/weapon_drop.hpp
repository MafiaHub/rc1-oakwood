#pragma once

inline auto drop_entitycreate(librg_event_t* evnt) -> void {
	auto drop = (mafia_weapon_drop *)evnt->entity->user_data;
	librg_data_wptr(evnt->data, drop->model, sizeof(char) * 32);
	librg_data_wptr(evnt->data, &drop->weapon, sizeof(inventory_item));
}

inline auto drop_entityremove(librg_event_t* evnt) -> void {
}
