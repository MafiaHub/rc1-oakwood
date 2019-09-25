premake.path = premake.path .. ";Build"
workspace "Oakwood"
    configurations { "Debug", "Release", "Production" }

    pic "On"
    symbols "On"

    if os.istarget('windows') then
        staticruntime "On"
    end

    startproject "Launcher"
    characterset "MBCS"
    cppdialect "c++17"

    location "../Build/"
    debugdir "../Bin/%{cfg.buildcfg}/"
    targetdir "../Bin/%{cfg.buildcfg}/"

    -- general configuration definition
    filter "platforms:x64"
         architecture "x86_64"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG", "GC_DBG", "OAK_BUILD_CHANNEL=1" }
        optimize "Off"
        runtime "Debug"

    filter "configurations:Release or Production"
        defines { "OAK_BUILD_CHANNEL=3" }
        optimize "Off" -- Optimization is disabled due to client issues
		runtime "Release"

    -- disabling as less warnings as possible
    filter "action:vs*"

    if os.istarget('windows') then
        -- causes D9025 warning
        -- buildoptions "/std:c++latest"
    else
        buildoptions "-std=c++17"
    end

    filter {}

    defines {
        "_CRT_SECURE_NO_WARNINGS"
        -- "NOMINMAX",
        --"WIN32_LEAN_AND_MEAN"
    }

    includedirs {
        ".",
        "./Shared",
    }

    --
    -- Source subprojects
    --

    dofile("../Vendors/premake5.lua")()

    if os.target() == "windows" then
        group "Client"

        -- additional tools
        if os.target() == "windows" then
            dofile("../Tools/premake/helpers/winsdk.lua")
        end

        include "Launcher"
        include "Client"

        group "Extensions"

        include "WidescreenFix"
    end

    group "Server"
    include "Server"
