#pragma once
zpl_global bool web_server_running = true;
zpl_global struct mg_mgr mgr;
zpl_global struct mg_connection *nc;
static struct mg_serve_http_opts s_http_server_opts;

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
    if (ev == MG_EV_HTTP_REQUEST) {
        mg_serve_http(nc, (struct http_message *)p, s_http_server_opts);
    }
}

void webserver_start() {
    
    mg_mgr_init(&mgr, NULL);
    nc = mg_bind(&mgr, std::to_string(GlobalConfig.port).c_str(), ev_handler);
    if (nc == NULL) {
        printf("[Oakwood MP] Failed to create listener for webserver check ports !\n");
        return;
    }

    // Set up HTTP server parameters
    mg_set_protocol_http_websocket(nc);
    s_http_server_opts.document_root = "static";
    s_http_server_opts.enable_directory_listing = "yes";
    
    auto web_loop = [=]() { 
        while (web_server_running) {
            mg_mgr_poll(&mgr, 200);
        }

        mg_mgr_free(&mgr);
    };

    std::thread web_thread(web_loop);
    web_thread.detach();
}

void webserver_stop() {
    web_server_running = false;
}
