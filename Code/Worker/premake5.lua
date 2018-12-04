project "Worker"
    language "C++"
    kind "WindowedApp"

    targetname "OakwoodWorker"
    characterset "Unicode"
    flags "NoManifest"

    filter { "system:windows", "kind:not StaticLib" }
        linkoptions "/manifestdependency:\"type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\""

    vpaths { ["*"] = "*" }

    libdirs {
        "../../Bin/%{cfg.buildcfg}/bin"
    }

    includedirs
    {
        "./include",
        ".",

        "../Shared",
        "../../Vendors/cef/",
    }

    filter { "system:windows" }
        links {
            "delayimp"
        }
        linkoptions "/DELAYLOAD:libcef.dll"

    links {
        "libcef",
        "cefwrapper",
    }

    files {
        "premake5.lua",
        "../../Vendors/*.h",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc",
        "**.manifest"
    }

    postbuildcommands {
        'mt.exe -manifest "../Scripts/manifest.xml" -outputresource:"../Bin/%{cfg.buildcfg}/OakwoodWorker.exe"',
    }

    filter "files:ui/**"
        flags { "ExcludeFromBuild" }
