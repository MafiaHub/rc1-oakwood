#pragma once

enum {
    OAK_BUILD_INDEV,
    OAK_BUILD_TESTING,
    OAK_BUILD_RELEASE,
};

static const char *oak_build_channel[3] = {
    "Development",
    "Testing",
    "Release",
};

#define OAK_BUILD_MAGIC 0xDEADC0DEDEADBEEF
#define OAK_BUILD_VERSION 1
#define OAK_BUILD_VERSION_STR "RC1"
#define OAK_BUILD_CHANNEL OAK_BUILD_INDEV
#define OAK_BUILD_DATE __DATE__
#define OAK_BUILD_TIME __TIME__
