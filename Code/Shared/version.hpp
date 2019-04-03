#pragma once

enum {
    OAK_BUILD_INDEV = 1,
    OAK_BUILD_TESTING,
    OAK_BUILD_RELEASE,
};

static const char *oak_build_channel[4] = {
    "Undefined",
    "Development",
    "Testing",
    "Release",
};

inline auto get_version_hash(int ver) {
    static zpl_u64 hash = 0;

    if (hash == 0)
        hash = zpl_fnv64(&ver, sizeof(ver));

    return hash;
}

#define OAK_BUILD_MAGIC (u64)0xDEADC0DEDEADBEEF
#define OAK_BUILD_VERSION_BASE 4
#define OAK_BUILD_VERSION get_version_hash(OAK_BUILD_VERSION_BASE)
#define OAK_BUILD_VERSION_STR "RC1"
#define OAK_BUILD_CHANNEL 1
#define OAK_BUILD_DATE __DATE__
#define OAK_BUILD_TIME __TIME__
