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
        "{COPY} " .. "../Files/ ../Bin/Debug",
        "{COPY} " .. "../Files/ ../Bin/Release",
	}

    linkoptions {
        "/SAFESEH:NO"
    }
