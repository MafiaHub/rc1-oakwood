struct mafia_settings {
    u8 magic_number;

    u16 width;
    u16 unk2;
    u16 height;
    u16 unk3;

    u16 bitdept;
    u8 unk4[6];
    u8 antialiasing;
    u8 unk5[2];

    bool fullscreen;
};

extern mafia_settings settings;

/* settings */
constexpr const char *OAKWOOD_CONFIG_NAME = "config/launcher.json";
constexpr const char *OAKWOOD_CONFIG_DATA = R"foo(
{
    /* path to game folder */
    "gamepath": "C:/Program Files/Steam/steamapps/common/Mafia/Mafia/",

    /* game settings */
    "width": 1600,
    "height": 900,
    "fullscreen": false,
    "antialiasing": 4,
}
)foo";
