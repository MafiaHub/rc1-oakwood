project "WidescreenFix"
    language "C++"
    kind "SharedLib"

    toolset "v142"

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
