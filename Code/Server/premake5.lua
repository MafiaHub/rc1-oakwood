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
        "../../Vendors/md5/md5.c",
        "../../Vendors/*.h",
        "../../Vendors/*.hpp",
        "../../Vendors/librg/*.h",
        "../../Vendors/librg/*.hpp",
        "../Shared/*.hpp",
        "../Shared/*.cpp",
        "../../Vendors/angelscript/addon/*/*.h",
        "../../Vendors/angelscript/addon/*/*.cpp"
    }

    includedirs {
        "../Shared",
        "../../Vendors/librg",
        "../../Vendors/semver",
        "../../Vendors",
        "../../Vendors/nanomsg/include",
        "../../Vendors/md5",
        "../../Vendors/angelscript/include"
    }

    libdirs {
        "../../Vendors/nanomsg/lib",
        "../../Vendors/curl",
        "../../Vendors/angelscript/lib"
    }

    prebuildcommands {
        "node ../Scripts/bridge.js"
    }

    configuration "windows"
        toolset "v142"
        filter "configurations:Debug"
            links {
                "GeoIP",
                "Crypt32",
                "libcurl",
                "nanomsg32",
                "ws2_32",
                "mswsock",
                "advapi32",
                "angelscriptd"
            }

        filter "configurations:Release or Production or Pre-Release"
        links {
            "GeoIP",
            "Crypt32",
            "libcurl",
            "nanomsg32",
            "ws2_32",
            "mswsock",
            "advapi32",
            "angelscript"
        }
        

    configuration "linux or macosx"
        links {
        --    "libcurl",
            "nanomsg",
            "GeoIP",
            "pthread",
            "dl",
            "angelscript"
        }
