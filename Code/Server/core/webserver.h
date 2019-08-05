zpl_global bool web_server_running = true;
zpl_global struct mg_mgr mgr;
zpl_global struct mg_connection *nc;
zpl_global zpl_thread web_thread;

static struct mg_serve_http_opts s_http_server_opts;

static void oak__webserver_ev_handler(struct mg_connection *nc, int ev, void *p) {
    if (ev == MG_EV_HTTP_REQUEST) {
        mg_serve_http(nc, (struct http_message *)p, s_http_server_opts);
    }
}

static void oak__webserver_handle_info(struct mg_connection* nc, int ev, void* p) {
    char buf[512] = { 0 };

    oak_endp_payload_info(buf);

    mg_printf(nc, "HTTP/1.0 200 OK\r\n\r\n%s", buf);
    nc->flags |= MG_F_SEND_AND_CLOSE;
}

zpl_isize oak__webserver_runner(struct zpl_thread *t) {
    while (web_server_running) {
        mg_mgr_poll(&mgr, 1);
    }

    mg_mgr_free(&mgr);

    return 0;
}

void oak_webserver_init() {
    oak_log("[info] Initializing webserver at :%d...\n", GlobalConfig.port);
    mg_mgr_init(&mgr, NULL);
    nc = mg_bind(&mgr, zpl_bprintf("%d", (int)GlobalConfig.port), oak__webserver_ev_handler);

    if (nc == NULL) {
        oak_log("[error] Failed to create listener for webserver, check ports !\n");
        return;
    }

    // Set up HTTP server parameters
    mg_set_protocol_http_websocket(nc);
    s_http_server_opts.document_root = "static";
    s_http_server_opts.enable_directory_listing = "yes";

    mg_register_http_endpoint(nc, "/info", oak__webserver_handle_info);

    zpl_thread_init(&web_thread);
    zpl_thread_start(&web_thread, oak__webserver_runner, NULL);
}

void oak_webserver_stop() {
    oak_log("Stopping the webserver...");
    web_server_running = false;
    zpl_thread_destroy(&web_thread);
}
