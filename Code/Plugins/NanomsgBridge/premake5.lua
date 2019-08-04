project "NanomsgBridge"
    language "C++"
    kind "SharedLib"
    targetdir "../../../Bin/%{cfg.buildcfg}/plugins/"
    vpaths { ["*"] = "*" }

    files {
        "premake5.lua",
        "../../Shared/Oakwood/*.cpp",
        "../../Shared/Oakwood/*.hpp",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc"
    }

    libdirs {
        "../../../Vendors/nanomsg/lib",
    }

    includedirs {
        "../../Shared",
        "../../../Vendors/nanomsg/include",
        "../../../Vendors/librg",
        "../../../Vendors",
    }

    -- TODO: add support for 64 bit versions
    configuration "windows"
        links {
            "nanomsg32",
            "ws2_32",
            "mswsock",
            "advapi32",
        }

    configuration "linux or macosx"
        links {
            "nanomsg",
            "pthread",
            "dl"
        }

