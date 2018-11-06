project "SampleMod"
    language "C++"
    kind "SharedLib"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
		"../Shared/Oakwood/*.cpp",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc"
    }