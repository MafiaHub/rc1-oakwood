#ifndef OAK_VERSION_H
#define OAK_VERSION_H

#include "semver.h"

static semver_t oak__version = {0};
static const char *oak__build_channel[4] = {
    "Undefined",
    "Development",
    "Testing",
    "Release",
};

#define OAK_NAME "Oakwood"
#define OAK_VERSION "1.24.0"

#define OAK_VERSION_SEMVER oak__version
#define OAK_VERSION_MAJOR oak__version.major
#define OAK_VERSION_MINOR oak__version.minor
#define OAK_VERSION_PATCH oak__version.patch

#ifndef OAK_BUILD_CHANNEL
#define OAK_BUILD_CHANNEL 1
#endif

#define OAK_BUILD_TYPE oak__build_channel[OAK_BUILD_CHANNEL]
#define OAK_BUILD_DATE __DATE__
#define OAK_BUILD_TIME __TIME__


#endif // OAK_VERSION_H
