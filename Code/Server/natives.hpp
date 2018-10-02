#pragma once

extern "C" {
    
    //
    // General
    //

    void oak_register_event(u64 type, zpl_event_cb cb) {
        zpl_event_add(&gamemode_events, (u64)type, cb);
    }

    void oak_print_test() {
        printf("It works!");
    }

    void oak_broadcast_msg_color(const char* text, u32 color) {
        librg_send(&network_context, NETWORK_SEND_CONSOLE_MSG, data, {
            librg_data_wu32(&data, strlen(text));
            librg_data_wu32(&data, color);
            librg_data_wptr(&data, (void*)text, strlen(text));
        });
    }

    void oak_broadcast_msg(const char* text) {
        oak_broadcast_msg_color(text, 0xFFFFFF);
    }

    //
    // Player
    //

    void oak_player_inventory_add(librg_entity_t *entity, inventory_item *item) {
        player_inventory_add(entity, item);
    }

    void oak_player_spawn(librg_entity_t *entity) {
        player_send_spawn(entity);
    }

    void oak_player_respawn(librg_entity_t *entity) {
        player_send_respawn(entity);
    }
}

auto set_up_natives() -> void {
    auto vt = &gm.vtable;

    vt->register_event = oak_register_event;
    vt->print_test = oak_print_test;
    vt->broadcast_msg = oak_broadcast_msg;
    vt->broadcast_msg_color = oak_broadcast_msg_color;

    vt->player_inventory_add = oak_player_inventory_add;
    vt->player_spawn = oak_player_spawn;
    vt->player_respawn = oak_player_respawn;
}