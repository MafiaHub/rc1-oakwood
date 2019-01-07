project "SampleMod"
    language "C++"
    kind "SharedLib"
    targetdir "../../../Bin/%{cfg.buildcfg}/plugins/"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
	    "../../Shared/Oakwood/*.cpp",
        "../../Shared/Oakwood/*.hpp",    
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc"
    }

    configuration "linux or macosx"
        links {
            "pthread",
            "dl"
        }

