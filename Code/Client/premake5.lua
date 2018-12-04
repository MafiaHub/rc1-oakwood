project "Client"
    language "C++"
    kind "SharedLib"

    targetname "OakwoodClient"

    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc",
        "../../Vendors/*.h",
        "../../Vendors/*.hpp",
        "../Shared/*.hpp",
        "../Shared/*.cpp",
        "**.manifest"
    }

    flags "NoManifest"
    filter { "system:windows", "kind:not StaticLib" }
        linkoptions "/manifestdependency:\"type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\""

    includedirs {
        "../../Vendors/cef/"
    }

    libdirs {
        "../../Bin/%{cfg.buildcfg}/bin"
    }

	links {
		"bass",
		"d3d9",
		"d3dx9",
		"detours",
		"lua",
		"opus",
		"dxguid",
		"dinput8",
        "libcef",
        "cefwrapper"
	}

	postbuildcommands {
		'mt.exe -manifest "../Scripts/manifest.xml" -outputresource:"../Bin/%{cfg.buildcfg}/OakwoodClient.dll"',

        -- copy additional files and stuff
        "{COPY} " .. "../Files/ ../Bin/Debug",
        "{COPY} " .. "../Files/ ../Bin/Release",
	}

    linkoptions {
        "/SAFESEH:NO"
    }
