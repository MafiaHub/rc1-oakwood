return function(version, unity_build)
    local cefinstall = "../Vendors/cef"
    local cefwrapdir = cefinstall .. "/libcef_dll"
    local cefwrapper = function()
        project "CefWrapper"
            language "C++"
            kind "StaticLib"

            targetname "CefWrapper"
            targetdir "../Bin/%{cfg.buildcfg}/"

            defines {
                "WIN32", "_WIN32", "_WINDOWS",              -- Windows platform
                "UNICODE", "_UNICODE",                      -- Unicode build
                "WINVER=0x0601", "_WIN32_WINNT=0x601",      -- Targeting Windows 7
                "NOMINMAX",                                 -- Use the standard's templated min/max
                "WIN32_LEAN_AND_MEAN",                      -- Exclude less common API declarations
                "_HAS_EXCEPTIONS=0",                        -- Disable exceptions
                "WRAPPING_CEF_SHARED",
                "USING_CEF_SHARED",
            }

            includedirs {
                cefwrapdir,
                cefwrapdir .. "/../",
            }

            if unity_build == true then
                files {
                    cefwrapdir .. "/**.hpp",
                    cefwrapdir .. "/**.h",
                    cefwrapdir .. "/cerwrapper.cc",
                }

                -- single source cef wrapper (unity build)
                local cnt = ""
                local p = os.matchfiles(cefwrapdir .. "/**.cc")
                for _, v in pairs(p) do
                    local name = v:gsub(cefwrapdir .. "/", "")
                    if name ~= "cerwrapper.cc" then
                        cnt = cnt .. "#include \"" .. name .. "\"\n"
                    end
                end
                io.writefile(cefwrapdir .. "/cerwrapper.cc", cnt)
            else
                files {
                    cefwrapdir .. "/**.hpp",
                    cefwrapdir .. "/**.h",
                    cefwrapdir .. "/**.cc",
                    cefwrapdir .. "/**.cpp",
                }
            end
    end

    -- make sure we have our cache folder
    local cachedir = "../Tools/premake/cache/cef/"
        os.mkdir(cachedir)

    local function download(cachedir)
        if os.isdir(cachedir) and os.isfile(cachedir .. version .. ".tar.bz2") then
            print("CEF: " .. version .. " is already cached, skipping download")
        else
            print("CEF: downloading from " .. "http://opensource.spotify.com/cefbuilds/" .. version .. ".tar.bz2")

            local last_value = 0
            local result, code = http.download(
                "http://opensource.spotify.com/cefbuilds/" .. version .. ".tar.bz2",
                cachedir .. version .. ".tar.bz2",
                {
                    progress = function(total, current)
                        local value = math.floor(math.min(math.max(current / total, 0), 1) * 100.0)
                        if value > last_value then
                            last_value = value
                            print("CEF: cef download progress " .. value  .. " / 100.")
                        end
                    end,
                }
            )
        end
    end

    local function decompress(cachedir)
        if os.isdir(cachedir) and os.isdir(cachedir .. version) then
            print("CEF: unpacked " .. version .. " is already cached, skipping decompression")
        else
            print("CEF: starting bz2 decompression ...")
            local absolute  = path.getabsolute('') .. "/"
            local extractor = absolute .. "/../Tools/premake/bin/win32/7z.exe"

            local source = absolute .. cachedir .. version .. ".tar.bz2"
            local destin = absolute .. cachedir

            os.executef("%s e %s -o%s *.tar", extractor, source, destin)

            source = absolute .. cachedir .. version .. ".tar"
            destin = absolute .. cachedir

            os.executef("%s x %s -o%s %s\\*.* -r", extractor, source, destin, version)
        end
    end

    local function install(cachedir, installdir)
        if os.isdir(installdir) then
            print("CEF: removing old installation...")
            os.rmdir(installdir)
        end

        print("CEF: copying from cache...")
        os.execute("{COPY} " .. cachedir .. version .. " " .. installdir)
        io.writefile(installdir .. "/" .. version .. ".ver", "") -- create a .ver file (version lock)
    end

    -- start up downloading and upacking
    if os.isdir(cefinstall) and os.isfile(cefinstall .. "/" .. version .. ".ver") then
        print("CEF: proper version is already installed, skipping")
    else
        download(cachedir)
        decompress(cachedir)
        install(cachedir, cefinstall)
    end

    if not os.isfile("../Bin/Debug/bin/icudtl.dat") then
        -- copy structure
        print("CEF: copying binaries and files")

        os.execute("{COPY} " .. cefinstall .. "/Debug ../Bin/Debug/bin")
        os.execute("{COPY} " .. cefinstall .. "/Release ../Bin/Release/bin")

        os.execute("{COPY} " .. cefinstall .. "/Resources/icudtl.dat ../Bin/Debug/bin")
        os.execute("{COPY} " .. cefinstall .. "/Resources/icudtl.dat ../Bin/Release/bin")

        os.execute("{COPY} " .. cefinstall .. "/Resources/ ../Bin/Debug/cef")
        os.execute("{COPY} " .. cefinstall .. "/Resources/ ../Bin/Release/cef")

        os.remove("../Bin/Debug/cef/icudtl.dat")
        os.remove("../Bin/Release/cef/icudtl.dat")
    end

    -- generate cef wrapper
    cefwrapper()
end
