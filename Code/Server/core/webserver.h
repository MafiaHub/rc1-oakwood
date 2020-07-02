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

#ifdef WIN32
#define SYS_TYPE "Win-VS2019"
#elif __APPLE__
#define SYS_TYPE "MacOS"
#elif __MINGW32__
#define SYS_TYPE "Win-MinGW-x86)"
#elif __MINGW64__
#define SYS_TYPE "Win-MinGW-x64)"
#elif __linux__
#define SYS_TYPE "Linux"
#endif

    mg_printf(nc, "HTTP/1.0 200 OK\nServer: Oakwood/%s (%s)\nContent-Length: %d\nContent-Type: application/json\nConnection: Closed\r\n\r\n%s", OAK_VERSION, SYS_TYPE, strlen(buf), buf);
    nc->flags |= MG_F_SEND_AND_CLOSE;
}

static void oak__webserver_handle_files(struct mg_connection* nc, int ev, void* p) {
    std::string filelist = "";

    std::string json;

    if (jfiles.size() >= 1)
    {
        for (int i = 0; i < jfiles.size(); i++) {
            filelist += "    {\n        \"name\": \"" + jfiles[i].first + "\", \n        \"hash\": \"" + jfiles[i].second + "\"\n    }";

            if (i + 1 != jfiles.size())
                filelist += ",\n";
        }

        json = "{\n    \"files\": [\n" + filelist + "]\n}";
    }
    else
    {
        json = "";
    }

    #ifdef WIN32
        #define SYS_TYPE "Win32"
    #elif __APPLE__
        #define SYS_TYPE "MacOS"
    #elif __linux__
        #define SYS_TYPE "Linux"
    #endif

    mg_printf(nc, "HTTP/1.0 200 OK\nServer: Oakwood/%s (%s)\nContent-Length: %d\nContent-Type: application/json\nConnection: Closed\r\n\r\n%s", OAK_VERSION, SYS_TYPE, json.size(), json.c_str());
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
    oak_log("^F[^5INFO^F] Initializing webserver at port ^C%d^F...^R\n", GlobalConfig.port);
    mg_mgr_init(&mgr, NULL);
    nc = mg_bind(&mgr, zpl_bprintf("%d", (int)GlobalConfig.port), oak__webserver_ev_handler);

    if (nc == NULL) {
        oak_log("^F[^9ERROR^F] Failed to create listener for webserver, check ports !^R\n");
        return;
    }

    // Set up HTTP server parameters
    mg_set_protocol_http_websocket(nc);
    s_http_server_opts.document_root = "static";
    s_http_server_opts.enable_directory_listing = "yes";

    mg_register_http_endpoint(nc, "/info", oak__webserver_handle_info);
    mg_register_http_endpoint(nc, "/files", oak__webserver_handle_files);

    zpl_thread_init(&web_thread);
    zpl_thread_start(&web_thread, oak__webserver_runner, NULL);
}

void oak_webserver_stop() {
    oak_log("^F[^5INFO^F] ^9Stopping ^Fthe webserver...^R\n");
    web_server_running = false;
    zpl_thread_destroy(&web_thread);
}
