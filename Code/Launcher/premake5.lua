project "Launcher"
    language "C++"
    kind "WindowedApp"

    toolset "v142"

    flags { "NoIncrementalLink" }
    editandcontinue "Off"

    filter "configurations:Debug"
        defines { "OAKWOOD_CONSOLE=1" }

    filter {}

    targetname "Oakwood"
    characterset "Unicode"
    linkoptions "/IGNORE:4254 /ENTRY:wmainCRTStartup /OPT:NOLBR /SAFESEH:NO /DYNAMICBASE:NO /LARGEADDRESSAWARE /LAST:.oakwood"

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
