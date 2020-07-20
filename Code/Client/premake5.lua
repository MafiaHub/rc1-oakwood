project "Client"
    language "C++"
    kind "SharedLib"

    targetname "OakwoodClient"
    vpaths { ["*"] = "*" }

    toolset "v142"

    files {
        "premake5.lua",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc",
        "../../Vendors/semver/semver.c",
        "../../Vendors/md5/md5.c",
        "../../Vendors/*.h",
        "../../Vendors/*.hpp",
        "../Shared/*.hpp",
        "../Shared/*.cpp",
        "**.manifest",
        "../../Vendors/angelscript/addon/*/*.h",
        "../../Vendors/angelscript/addon/*/*.cpp"
    }

    defines {
        "CURL_STATICLIB"
    }


    includedirs {
        "../../Vendors/semver",
        "../../Vendors/md5",
        "../../Vendors/bass"
    }

    flags "NoManifest"
    libdirs {
        "../../Bin/%{cfg.buildcfg}/bin"
    }

	filter "configurations:Debug"
            links {
                "libcurl",
                "Ws2_32",
                "Wldap32",
                "Normaliz",
                "Crypt32",
		        "d3d9",
		        "d3dx9",
		        "detours",
		        "lua",
		        "dxguid",
                "wbemuuid",
                "advapi32",
                "dinput8",
                "bass",
                "angelscriptd"
            }

        filter "configurations:Release or Production or Pre-Release"
            links {
                "libcurl",
                "Ws2_32",
                "Wldap32",
                "Normaliz",
                "Crypt32",
                "d3d9",
                "d3dx9",
                "detours",
                "lua",
                "dxguid",
                "wbemuuid",
                "advapi32",
                "dinput8",
                "bass",
                "angelscript"
            }

	postbuildcommands {
        -- copy additional files and stuff
        "{COPY} " .. "../Files/ ../Bin/%{cfg.buildcfg}",
	}

    linkoptions {
        "/SAFESEH:NO"
    }
