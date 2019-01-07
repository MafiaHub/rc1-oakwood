project "SampleMod"
    language "C++"
    kind "SharedLib"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
	    "../Shared/Oakwood/*.cpp",
        "../Shared/Oakwood/*.hpp",    
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc"
    }

    filter "configurations:Debug"
        postbuildcommands {
            "{MOVE} " .. "../Bin/Debug/SampleMod.dll ../Bin/Debug/plugins/SampleMod.dll"
        }

    filter "configurations:Release"
        postbuildcommands {
            "{MOVE} " .. "../Bin/Release/SampleMod.dll ../Bin/Release/plugins/SampleMod.dll"
        }

    configuration "linux or macosx"
        links {
            "pthread",
            "dl"
        }

