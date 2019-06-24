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

    linkoptions {
        "/SAFESEH:NO"
    }
