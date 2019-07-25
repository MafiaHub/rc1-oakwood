project "Server"
    language "C++"
    targetname "OakwoodServer"
    kind "ConsoleApp"
    vpaths { ["*"] = "*" }
    files {
        "premake5.lua",
        "**.h",
        "**.hpp",
        "**.cpp",
        "**.rc",
        "../../Vendors/http/mongoose.c",
        "../../Vendors/http/mongoose.h",
        "../../Vendors/*.h",
        "../../Vendors/*.hpp",
        "../../Vendors/librg/*.h",
        "../../Vendors/librg/*.hpp",
        "../Shared/*.hpp",
        "../Shared/*.cpp"
    }
    includedirs {
        "../Shared",
        "../../Vendors/librg",
        "../../Vendors"
    }

    configuration "linux or macosx"
        links {
            "pthread",
            "curses",
            "dl"
        }
