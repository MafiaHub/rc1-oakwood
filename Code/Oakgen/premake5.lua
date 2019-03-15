project "Oakgen"
    language "C++"
    kind "ConsoleApp"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
        "**.cc",
        "../../Vendors/librg/zpl.h"
    }
    includedirs {
        "../Shared",
        "../../Vendors/librg",
        "../../Vendors"
    }
    postbuildcommands {
		"{COPY} ../Bin/%{cfg.buildcfg}/Oakgen* ../Tools/"
	}
	configuration "Debug"
		debugdir "Bin/Debug"

    configuration "linux or macosx"
        links {
            "pthread",
            "dl"
        }