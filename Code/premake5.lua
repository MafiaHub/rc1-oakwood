premake.path = premake.path .. ";Build"
workspace "Oakwood"
    configurations { "Debug", "Release", "Production" }

    pic "On"
    symbols "On"
    staticruntime "On"

    startproject "Launcher"
    characterset "MBCS"

    location "../Build/"
    debugdir "../Bin/%{cfg.buildcfg}/"
    targetdir "../Bin/%{cfg.buildcfg}/"

    -- general configuration definition
    filter "platforms:x64"
         architecture "x86_64"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG", "GC_DBG" }
        optimize "Off"
        runtime "Debug"

    filter "configurations:Release or Production"
        optimize "Off" -- Optimization is disabled due to client issues
		runtime "Release"

    -- disabling as less warnings as possible
    filter "action:vs*"

    if os.istarget('windows') then
        buildoptions "/std:c++latest"
    else
        buildoptions "-std=c++17"
    end

    filter {}

    defines {
        "NOMINMAX",
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
        include "WidescreenFix"
    end

    group "Plugins"

    include "Plugins/Freeride"
    include "Plugins/EscapeTheCops"
    -- include "Plugins/LuaMod"

    group "Server"

    include "Server"
    include "Oakgen"
