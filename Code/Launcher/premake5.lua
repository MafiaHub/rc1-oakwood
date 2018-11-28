project "Launcher"
    language "C++"
    kind "WindowedApp"

    flags { "NoIncrementalLink" }
    editandcontinue "Off"

    targetname "oakwood"
    characterset "Unicode"
    linkoptions "/IGNORE:4254 /ENTRY:wmainCRTStartup /OPT:NOLBR /SAFESEH:NO /DYNAMICBASE:NO /LARGEADDRESSAWARE /LAST:.zdata"

    icon "icon.ico"
    vpaths { ["*"] = "*" }

    includedirs {
        ".",
    }

    files {
        "premake5.lua",
        "**.rc",
        "**.ico",
        "**.h",
        "**.hpp",
        "win32.cpp",
        "launcher.cpp",
        "loader/*.*",
    }
