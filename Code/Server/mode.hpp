#pragma once

struct mode_event {
    librg_entity_t *player_ent;
	librg_event_t *evnt;
	mafia_player *player;
} mode_data;

enum mod_mode_events {
    MODE_ON_PLAYER_CONNECTED,
    MODE_ON_PLAYER_DISCONNECTED,
    MODE_ON_PLAYER_DIED,
};

auto mode_prepare_data() {
    mode_event data_ = {0};
    mode_event *dptr = &mode_data;
    *dptr = data_;
}

auto mode_trigger(u64 id) -> void {
    zpl_event_trigger(&gamemode_events, id, (zpl_event_data *)&mode_data);
}

auto init_event_handler() {
	zpl_event_init(&gamemode_events, zpl_heap());
}
