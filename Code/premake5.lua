premake.path = premake.path .. ";Build"

function os.winSdkVersion()
    local reg_arch = iif( os.is64bit(), "\\Wow6432Node\\", "\\" )
    local sdk_version = os.getWindowsRegistry( "HKLM:SOFTWARE" .. reg_arch .."Microsoft\\Microsoft SDKs\\Windows\\v10.0\\ProductVersion" )
    if sdk_version ~= nil then return sdk_version end
end

workspace "Oakwood"
    configurations { "Debug", "Release" }

    if os.istarget('windows') then
        buildoptions "/std:c++latest"
    else
        buildoptions "-std=c++17"
    end

    symbols "On"
    targetprefix ""
    characterset "MBCS"

    -- Enable position-independent-code generation
    pic "On"
    startproject "Server"

    location "../Build/"
    targetdir "../Bin/%{cfg.buildcfg}/"
    os.mkdir"../Build/symbols"

    defines {
        "NOMINMAX",
        --"WIN32_LEAN_AND_MEAN"
    }

    includedirs {
        ".",
        "./Shared",
        "../Vendors",
    }

    libdirs {
        "../Vendors/bass",
        "../Vendors/d3d9",
        "../Vendors/detours",
        "../Vendors/lua/lib",
        "../Vendors/opus/lib",
    }

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

    filter {"system:windows", "configurations:Release", "kind:not StaticLib"}
        linkoptions "/PDB:\"symbols\\$(ProjectName).pdb\""

    filter {"system:windows", "action:vs*"}
	if os.winSdkVersion() ~= nil then
        	systemversion(os.winSdkVersion() .. ".0")
	end

    -- Disable deprecation warnings and errors
    -- disabling as less warnings as possible
    filter "action:vs*"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            -- "_CRT_SECURE_NO_DEPRECATE",
            -- "_CRT_NONSTDC_NO_WARNINGS",
            -- "_CRT_NONSTDC_NO_DEPRECATE",
            -- "_SCL_SECURE_NO_WARNINGS",
            -- "_SCL_SECURE_NO_DEPRECATE",

            "_WINSOCK_DEPRECATED_NO_WARNINGS",
        }

    --
    -- Source subprojects
    --
    include "Client"
    include "Loader"
    include "LuaMod"
    include "SampleMod"
    include "Server"
    include "Oakgen"

-- Cleanup
if _ACTION == "clean" then
    os.rmdir("../Bin");
    os.rmdir("../Build");
end
