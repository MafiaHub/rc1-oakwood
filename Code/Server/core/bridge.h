#define OAK_BRIDGE_BUFFER 4096

#define NN_STATIC_LIB 1
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <nanomsg/reqrep.h>
#include <msgpack/cwpack.h>

int sock_out;
int sock_in;

/* GENERAL */

int oak_bridge_init() {
    oak_log("[info] initializing bridge...\n");

    sock_out = nn_socket(AF_SP, NN_PUB);
    sock_in = nn_socket(AF_SP, NN_REP);

    // TODO: move to config
    // const char *addr1 = "tcp://0.0.0.0:10101";
    // const char *addr2 = "tcp://0.0.0.0:10102";
    const char *addr1 = "ipc://oakwood-inbound";
    const char *addr2 = "ipc://oakwood-outbound";

    nn_bind(sock_out, addr1);
    nn_bind(sock_in, addr2);

    return 0;
}

int oak_bridge_free() {
    oak_log("[info] stopping bridge...\n");

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
    cw_pack_str(&pc, zpl_str_expand("player_connect"));
    cw_pack_signed(&pc, player);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
}

void oak_bridge_event_player_disconnect(oak_player player) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("player_disconnect"));
    cw_pack_signed(&pc, player);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
}

void oak_bridge_event_player_death(oak_player player) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("player_death"));
    cw_pack_signed(&pc, player);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
}

void oak_bridge_event_player_hit(oak_player player, oak_player attacker, float damage) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 4);
    cw_pack_str(&pc, zpl_str_expand("player_hit"));
    cw_pack_signed(&pc, player);
    cw_pack_signed(&pc, attacker);
    cw_pack_float(&pc, damage);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
}

void oak_bridge_event_player_key(oak_player player, int key) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 3);
    cw_pack_str(&pc, zpl_str_expand("player_key"));
    cw_pack_signed(&pc, player);
    cw_pack_signed(&pc, key);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
}

void oak_bridge_event_player_chat(oak_player player, oak_string text) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 3);
    cw_pack_str(&pc, zpl_str_expand("player_chat"));
    cw_pack_signed(&pc, player);
    cw_pack_str(&pc, text, zpl_strlen(text));
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
}

void oak_bridge_event_console(oak_string text) {
    printf("[info] executing server command: %s\n", text);
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("console"));
    cw_pack_str(&pc, text, zpl_strlen(text));
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
}

void oak_bridge_event_vehicle_destroy(oak_vehicle vehicle) {
    char buffer[OAK_BRIDGE_BUFFER] = {};
    cw_pack_context pc;
    cw_pack_context_init(&pc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_pack_array_size(&pc, 2);
    cw_pack_str(&pc, zpl_str_expand("vehicle_destroy"));
    cw_pack_signed(&pc, vehicle);
    nn_send(sock_out, buffer, pc.current - pc.start, 0);
}

