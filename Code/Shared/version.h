#ifndef OAK_VERSION_H
#define OAK_VERSION_H

#include "semver.h"
static semver_t oak__version = {0};

#define OAK_NAME "Oakwood"
#define OAK_VERSION "1.0.18"

#define OAK_VERSION_SEMVER oak__version
#define OAK_VERSION_MAJOR oak__version.major
#define OAK_VERSION_MINOR oak__version.minor
#define OAK_VERSION_PATCH oak__version.patch

#endif // OAK_VERSION_H
