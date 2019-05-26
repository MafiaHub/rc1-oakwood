project "WidescreenFix"
    language "C++"
    kind "SharedLib"

    targetname "WidescreenFix"
    targetdir "../../Bin/%{cfg.buildcfg}/bin"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc"
    }

    libdirs {
        "../../Bin/%{cfg.buildcfg}/bin"
    }

	postbuildcommands {
        -- copy additional files and stuff
        "{COPY} " .. "../Files/ ../Bin/Debug/Bin",
        "{COPY} " .. "../Files/ ../Bin/Release/Bin",
	}

    linkoptions {
        "/SAFESEH:NO"
    }
