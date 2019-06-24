project "Client"
    language "C++"
    kind "SharedLib"

    targetname "OakwoodClient"

    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc",
        "../../Vendors/*.h",
        "../../Vendors/*.hpp",
        "../Shared/*.hpp",
        "../Shared/*.cpp",
        "**.manifest"
    }

    filter "configurations:Production"
        defines { "OAK_BUILD_CHANNEL=3" }

    flags "NoManifest"
    libdirs {
        "../../Bin/%{cfg.buildcfg}/bin"
    }

	links {
		"d3d9",
		"d3dx9",
		"detours",
		"lua",
		"dxguid",
		"dinput8"
	}

	postbuildcommands {
        -- copy additional files and stuff
        "{COPY} " .. "../Files/ ../Bin/%{cfg.buildcfg}",
	}

    linkoptions {
        "/SAFESEH:NO"
    }
