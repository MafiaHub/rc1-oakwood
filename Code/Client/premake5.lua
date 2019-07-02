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

    filter "configurations:Production"
        toolset "v141_xp" -- TODO: update to the future releases
        defines { "OAK_BUILD_CHANNEL=3" }
