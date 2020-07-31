#define OAK_BRIDGE_BUFFER 4096

#define NN_STATIC_LIB 1
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <nanomsg/reqrep.h>
#include <nanomsg/survey.h>
#include <msgpack/cwpack.h>

int sock_out;
int sock_in;

/* GENERAL */

int oak_bridge_init() {
    oak_log("^F[^5INFO^F] Initializing bridge...^R\n");

    sock_out = nn_socket(AF_SP, NN_SURVEYOR);
    sock_in = nn_socket(AF_SP, NN_REP);

    const char *addr_in = oak_config_bridge_inbound_get();
    const char *addr_out = oak_config_bridge_outbound_get();

    /* set deadline for response */
    int deadline = 10;
    nn_setsockopt(sock_out, NN_SURVEYOR, NN_SURVEYOR_DEADLINE, &deadline, sizeof(deadline));

    nn_bind(sock_in, addr_out);
    nn_bind(sock_out, addr_in);

    return 0;
}

int oak_bridge_free() {
    oak_log("^F[^5INFO^F] Stopping bridge...^R\n");

    nn_close(sock_out);
    nn_close(sock_in);

    return 0;
}

int oak_bridge_tick() {
    char *buf;
    int bytes;

    do {
        bytes = nn_recv(sock_in, &buf, NN_MSG, NN_DONTWAIT);
        if (bytes < 1) { continue; }
        oak_bridge_router(buf, bytes);
        nn_freemsg(buf);
    } while (bytes > 0);

    return 0;
}

/* EVENTS */

void oak_bridge_event_player_connect(oak_player player) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("playerConnect"));
    cw_pack_signed(&pc, player);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_player_disconnect(oak_player player) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("playerDisconnect"));
    cw_pack_signed(&pc, player);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_player_death(oak_player player, oak_player killer, int reason, int type, int part) {
    char buffer[OAK_BRIDGE_BUFFER] = {0};
    cw_pack_context pc = {0};
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 6);
    cw_pack_str(&pc, zpl_str_expand("playerDeath"));
    cw_pack_signed(&pc, player);
    cw_pack_signed(&pc, killer);
    cw_pack_signed(&pc, reason);
    cw_pack_signed(&pc, type);
    cw_pack_signed(&pc, part);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_player_hit(oak_player player, oak_player attacker, float damage) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 4);
    cw_pack_str(&pc, zpl_str_expand("playerHit"));
    cw_pack_signed(&pc, player);
    cw_pack_signed(&pc, attacker);
    cw_pack_float(&pc, damage);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_player_key(oak_player player, int key, bool is_pressed) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 4);
    cw_pack_str(&pc, zpl_str_expand("playerKey"));
    cw_pack_signed(&pc, player);
    cw_pack_signed(&pc, key);
    cw_pack_signed(&pc, is_pressed);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_player_chat(oak_player player, const char *text) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 3);
    cw_pack_str(&pc, zpl_str_expand("playerChat"));
    cw_pack_signed(&pc, player);
    cw_pack_str(&pc, text, zpl_strlen(text));
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_console(const char *text) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("console"));
    cw_pack_str(&pc, text, zpl_strlen(text));
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_vehicle_destroy(oak_vehicle vehicle) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("vehicleDestroy"));
    cw_pack_signed(&pc, vehicle);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_vehicle_player_use(oak_vehicle vehicle, oak_player player, bool success, int seat_id, int entrance) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 6);
    cw_pack_str(&pc, zpl_str_expand("vehicleUse"));
    cw_pack_signed(&pc, vehicle);
    cw_pack_signed(&pc, player);
    cw_pack_signed(&pc, success);
    cw_pack_signed(&pc, seat_id);
    cw_pack_signed(&pc, entrance);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_dialog_done(oak_player player, int dialogId, int dialogSel, const char* dialogText) {
    char buffer[OAK_BRIDGE_BUFFER] = { 0 };
    cw_pack_context pc = { 0 };
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 5);
    cw_pack_str(&pc, zpl_str_expand("dialogClose"));
    cw_pack_signed(&pc, player);
    cw_pack_signed(&pc, dialogId);
    cw_pack_signed(&pc, dialogSel);
    cw_pack_str(&pc, zpl_str_expand(dialogText));
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}

void oak_bridge_event_vehicle_exploded(oak_vehicle vehicle) {
    char buffer[OAK_BRIDGE_BUFFER] = { 0 };
    cw_pack_context pc = { 0 };
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("vehicleExploded"));
    cw_pack_signed(&pc, vehicle);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
    nn_recv(sock_out, buffer, OAK_BRIDGE_BUFFER, 0);
}
