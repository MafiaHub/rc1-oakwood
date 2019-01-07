premake.path = premake.path .. ";Build"
workspace "Oakwood Plugins"
    configurations { "Debug", "Release" }

    pic "On"
    symbols "On"

    characterset "MBCS"

    location "../Build/"
    targetdir "../Bin/%{cfg.buildcfg}/plugins/"

    -- general configuration definition
    filter "platforms:x64"
         architecture "x86_64"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG", "GC_DBG" }
        optimize "Off"
        runtime "Debug"

    filter "configurations:Release"
        -- staticruntime "On"
        optimize "Off" -- Optimization is disabled due to client issues
		runtime "Release"

    -- disabling as less warnings as possible
    filter "action:vs*"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "_WINSOCK_DEPRECATED_NO_WARNINGS",
        }

    if os.istarget('windows') then
        buildoptions "/std:c++latest"
    else
        buildoptions "-std=c++17"
    end

    defines {
        "NOMINMAX",
        --"WIN32_LEAN_AND_MEAN"
    }

    includedirs {
        ".",
        "../shared",
        "../vendors/librg"
    }

    --
    -- Source subprojects
    --

    group "Plugins"

    -- include "LuaMod"
    include "SampleMod"
