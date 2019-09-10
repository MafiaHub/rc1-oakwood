#pragma once

#define json_apply(OBJ, VAR, NAME, STR, DEF)\
do { \
    zpl_json_object *NAME = zpl_json_find(OBJ, #NAME, false); \
    VAR = (NAME) ? NAME->STR : DEF; \
} while (0)

inline auto config_get(const char *filename, const char *default_cfg) -> zpl_json_object* {
    zpl_path_mkdir("config", 0775);
    
    if (!zpl_fs_exists(filename)) {
        printf("No JSON config found! Generating...");
        zpl_file default_file = { 0 };
        zpl_file_create(&default_file, filename);
        zpl_file_write(&default_file, default_cfg, zpl_strlen(default_cfg));
        zpl_file_close(&default_file);
    }

    zpl_file_contents json_file = zpl_file_read_contents(zpl_heap(), true, filename);

    u8 ok = 0;
    zpl_string json_config_data = zpl_string_make(zpl_heap(), (const char*)json_file.data);
    zpl_json_object json_config_ = { 0 };
    zpl_json_object *json_config = (zpl_json_object *)zpl_malloc(sizeof(zpl_json_object));
    *json_config = json_config_;
    zpl_json_parse(json_config, zpl_string_length(json_config_data), json_config_data, zpl_heap(), true, &ok);
    zpl_file_free_contents(&json_file);
    
    return json_config;
}
