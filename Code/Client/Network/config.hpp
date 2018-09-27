#pragma once

#define json_apply(VAR, NAME, STR, DEF)\
do { \
    zpl_json_object *NAME;\
    zpl_json_find(&json_config, #NAME, false, &NAME);\
    VAR = (NAME && NAME->STR) ? NAME->STR : DEF; \
} while (0)

struct _GlobalConfig {
	std::string server_address;
	std::string username;
} GlobalConfig;

constexpr const char* config_file_name = "multiplayer.json";

static const char *mod_default_config = R"foo(
{
		"username": "Unknown",
		"ip": "localhost"
}
)foo";

inline auto config_get() {

    if (!zpl_file_exists(config_file_name)) {
        mod_debug("No JSON config found! Generating...");
        zpl_file default_file = { 0 };
        zpl_file_create(&default_file, config_file_name);
        zpl_file_write(&default_file, mod_default_config, zpl_strlen(mod_default_config));
        zpl_file_close(&default_file);
    }

    zpl_file_contents json_file = zpl_file_read_contents(zpl_heap(), true, config_file_name);

    u8 ok = 0;
    zpl_string json_config_data = zpl_string_make(zpl_heap(), (const char*)json_file.data);
    zpl_json_object json_config = { 0 };
    zpl_json_parse(&json_config, zpl_string_length(json_config_data), json_config_data, zpl_heap(), true, &ok);
    zpl_file_free_contents(&json_file);

    if (ok == 0) {
        json_apply(GlobalConfig.server_address, ip, string, 0);
        json_apply(GlobalConfig.username, username, string, 0);
    } else mod_debug("JSON file is invalid!");
}

#undef json_apply