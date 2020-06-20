project "Server"
    language "C++"
    targetname "OakwoodServer"
    kind "ConsoleApp"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc",
        "../../Vendors/msgpack/cwpack.c",
        "../../Vendors/http/mongoose.c",
        "../../Vendors/http/mongoose.h",
        "../../Vendors/http/mongoose.h",
        "../../Vendors/semver/semver.c",
        "../../Vendors/*.h",
        "../../Vendors/*.hpp",
        "../../Vendors/librg/*.h",
        "../../Vendors/librg/*.hpp",
        "../Shared/*.hpp",
        "../Shared/*.cpp"
    }

    includedirs {
        "../Shared",
        "../../Vendors/librg",
        "../../Vendors/semver",
        "../../Vendors",
        "../../Vendors/nanomsg/include",
    }

    defines {
        "CURL_STATICLIB"
    }

    libdirs {
        "../../Vendors/nanomsg/lib",
        "../../Vendors/curl",
    }

    prebuildcommands {
        "node ../Scripts/bridge.js"
    }

    configuration "windows"
        links {
            "GeoIP",
            "Crypt32",
            "libcurl",
            "nanomsg32",
            "ws2_32",
            "mswsock",
            "advapi32",
        }

    configuration "linux or macosx"
        links {
        --    "libcurl",
            "nanomsg",
            "GeoIP",
            "pthread",
            "dl"
        }
