project "Client"
    language "C++"
    kind "SharedLib"

    targetname "oakwood-client"

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
        "../Shared/*.cpp"
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
    }

    linkoptions {
        "/SAFESEH:NO"
    }
