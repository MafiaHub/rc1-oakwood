#pragma once

zpl_global mg_context *web_context;

void webserver_start() {
    web_context = mg_start();
    auto port_str = std::to_string(GlobalConfig.port);
    mg_set_option(web_context, "ports", port_str.c_str());

    if (!zpl_file_exists("static"))
        zpl_path_mkdir("static", 0600);

    mg_set_option(web_context, "root", "static");
}

void webserver_stop() {
    mg_stop(web_context);
}