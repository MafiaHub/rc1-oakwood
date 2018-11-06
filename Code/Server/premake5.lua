project "Server"
    language "C++"
    kind "ConsoleApp"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
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
