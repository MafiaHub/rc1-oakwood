project "Client"
    language "C++"
    kind "SharedLib"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc"
    }
	links {
		"bass",
		"d3d9",
		"d3dx9",
		"detours",
		"lua",
		"opus",
		"dxguid",
		"dinput8",
	}
	postbuildcommands {
		"{COPY} ../Bin/%{cfg.buildcfg}/Client.dll ../../"
	}
    linkoptions {
        "/SAFESEH:NO"
    }