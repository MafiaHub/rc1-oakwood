#pragma once

std::vector<std::pair<std::string, std::string>> jfiles;

std::string get_hash(std::string filename)
{
    std::ifstream inBigArrayfile;
    inBigArrayfile.open(filename, std::ios::binary | std::ios::in);

    //Find length of file
    inBigArrayfile.seekg(0, std::ios::end);
    long Length = inBigArrayfile.tellg();
    inBigArrayfile.seekg(0, std::ios::beg);

    //read in the data from your file
    char* InFileData = new char[Length];
    inBigArrayfile.read(InFileData, Length);

    md5_state_t state;
    md5_byte_t digest[16];
    char hex_output[16 * 2 + 1];
    int di;

    md5_init(&state);
    md5_append(&state, (const md5_byte_t*)InFileData, Length);
    md5_finish(&state, digest);
    for (di = 0; di < 16; ++di)
        sprintf(hex_output + di * 2, "%02x", digest[di]);

    return std::string(hex_output);
}

std::pair<std::string, std::string> SplitFilename(const std::string& str)
{
    size_t found;
    found = str.find_last_of("/\\");

    return std::make_pair(str.substr(0, found), str.substr(found + 1));
}

std::vector<std::string> get_files(const std::string& s)
{
    std::vector<std::string> r;
    for (auto& p : std::filesystem::recursive_directory_iterator(s))
        r.push_back(p.path().string());
    return r;
}

std::vector<std::string> get_directories(const std::string& s)
{
    std::vector<std::string> r;
    for (auto& p : std::filesystem::recursive_directory_iterator(s))
        if (p.is_directory())
            r.push_back(p.path().string());
    return r;
}

void generate_list()
{
    std::vector<std::string> dirs = get_directories("static");

    for (auto dir : dirs)
    {
        auto p = SplitFilename(dir);
        auto d1 = p.second;
        auto n = p.second;

        std::transform(n.begin(), n.end(), n.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (n == "models" || n == "maps" || n == "sounds" || n == "anims")
        {
            auto files = get_files(dir);
            for (auto file : files)
            {
                auto p2 = SplitFilename(file);

                auto f = d1 + "/" + p2.second;

                auto h = get_hash(file);

                jfiles.push_back(std::make_pair(f, h));
            }
        }
        else if (n == "missions")
        {
            auto dirs2 = get_directories(dir);
            for (auto dir2 : dirs2)
            {
                auto p2 = SplitFilename(dir2);
                auto d2 = p2.second;
                auto files = get_files(dir2);
                for (auto file : files)
                {
                    auto p3 = SplitFilename(file);

                    auto f = d1 + "/" + d2 + "/" + p3.second;

                    auto h = get_hash(file);

                    jfiles.push_back(std::make_pair(f, h));
                }
            }
        }
    }

    oak_console_printf("^F[^5INFO^F] Loaded ^C%d ^Ffiles.\n^R", jfiles.size());
}