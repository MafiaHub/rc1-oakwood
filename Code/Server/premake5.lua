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
        "../Shared/*.hpp",
        "../Shared/*.cpp"
    }
    postbuildcommands {
        -- copy additional files and stuff
        "{COPY} " .. "../Files/plugins/ ../Bin/Debug/plugins",
        "{COPY} " .. "../Files/plugins/ ../Bin/Release/plugins",
    }
	configuration "Debug"
		debugdir "Bin/Debug"

    configuration "linux or macosx"
        links {
            "pthread",
            "dl"
        }
