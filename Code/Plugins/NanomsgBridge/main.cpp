#define ZPL_IMPLEMENTATION
#include "zpl.h"

#include <Oakwood/Framework.hpp>
#include <Oakwood/PlayerModelListing.hpp>

#define NN_STATIC_LIB 1
#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <nanomsg/pipeline.h>

#define REGISTER_METHOD_0(PATTERN, GM, METHOD) GM->METHOD([]() {char buffer[4096] = {0}; int size = zpl_snprintf(buffer, 4096, PATTERN, #METHOD);nn_send(sock_out, buffer, size, 0);});
#define REGISTER_METHOD_1(PATTERN, GM, METHOD, ARG1, CODE1) GM->METHOD([](ARG1) {char buffer[4096] = {0}; int size = zpl_snprintf(buffer, 4096, PATTERN, #METHOD, CODE1);nn_send(sock_out, buffer, size, 0);});
#define REGISTER_METHOD_2(PATTERN, GM, METHOD, ARG1, CODE1, ARG2, CODE2) GM->METHOD([](ARG1, ARG2) {char buffer[4096] = {0}; int size = zpl_snprintf(buffer, 4096, PATTERN, #METHOD, CODE1, CODE2);nn_send(sock_out, buffer, size, 0);});

//
// Entry point
//
GameMode *gm = nullptr;

int sock_out;
int sock_in;

OAK_MOD_MAIN /* (oak_api *mod) */ {
    // Set up mod information

    mod->name = "NanomsgBridge";
    mod->author = "MafiaHub Group";
    mod->version = "v1.0.0";

    // Initialize the GameMode
    gm = new GameMode(mod);

    zpl_printf("NanomsgBridge is starting...\n");

    sock_out = nn_socket(AF_SP, NN_PUB);
    sock_in = nn_socket(AF_SP, NN_PULL);

    char *addr1 = "tcp://0.0.0.0:10101";
    char *addr2 = "tcp://0.0.0.0:10102";

    nn_bind(sock_out, addr1);
    nn_bind(sock_in, addr2);

    REGISTER_METHOD_1("{\"name\":\"%s\",\"args\":[%d]}", gm, SetOnPlayerConnected, Player*p, p->GetId());
    REGISTER_METHOD_1("{\"name\":\"%s\",\"args\":[%d]}", gm, SetOnPlayerDisconnected, Player*p, p->GetId());
    REGISTER_METHOD_1("{\"name\":\"%s\",\"args\":[%d]}", gm, SetOnPlayerDied, Player*p, p->GetId());
    REGISTER_METHOD_1("{\"name\":\"%s\",\"args\":[%d]}", gm, SetOnVehicleDestroyed, Vehicle*p, p->GetId());
    REGISTER_METHOD_2("{\"name\":\"%s\",\"args\":[%d,\"%s\"]}", gm, SetOnPlayerChat, Player*p, p->GetId(), std::string s, s.c_str());

    gm->SetOnServerTick([]() {
        char *buf;
        int bytes;

        do {
            bytes = nn_recv(sock_in, &buf, NN_MSG, NN_DONTWAIT);
            if (bytes < 1) { continue; }

            // TODO:

            nn_freemsg(buf);
        } while (bytes > 0);

        nn_send(sock_out, "{\"name\":\"SetOnServerTick\"}", zpl_strlen("{\"name\":\"SetOnServerTick\"}"), 0);
    });
}

OAK_MOD_SHUTDOWN {
    zpl_printf("NanomsgBridge is shutting down...\n");

    nn_close(sock_out);
    nn_close(sock_in);
}
