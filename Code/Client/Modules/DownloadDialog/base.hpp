#include "Game/patches.hpp"

struct DlFile
{
    std::string name;
    std::string hash;
};

namespace dldialog
{
    extern std::vector<std::string> Split(std::string str, std::string by);

    int percent = 0, i = 0;
    double downloaded = 0, toDownload = 0;

    bool isDownloading = false;

    std::string jsonText, baseURL;

    char* title, * message, * button;

    std::vector<DlFile> filesToDownload;

    ServerInfo::ServerData server;

    std::string currentFileUrl;
    std::string currentFileName;
    std::string folder;

    std::string ip;
    int port;
    std::string mapname;

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

    inline void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if (from.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    bool fileExists(const std::string& name) {
        if (FILE* file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        }
        else {
            return false;
        }
    }

    void createDlList()
    {
        replaceAll(baseURL, "/files.json", "");

        std::string gpath = GlobalConfig.gamepath;

        std::string burl = baseURL;

        replaceAll(gpath, "//Game.exe", "");
        replaceAll(burl, "/files.txt", "");
        replaceAll(gpath, "/", "\\");

        std::string fileDir = gpath + "\\mods\\" + folder;
        modpath = "mods\\" + folder;

        CreateDirectory(fileDir.c_str(), NULL);

        filesToDownload.clear();
        auto fetched_list = jsonText;
        if (!fetched_list.empty()) {
            u8 failed = 0;
            zpl_string json_config_data = zpl_string_make(zpl_heap(), fetched_list.c_str());
            zpl_json_object json_master_data = { 0 };
            zpl_json_parse(&json_master_data, zpl_string_length(json_config_data), json_config_data, zpl_heap(), true, &failed);

            if (!failed) {
                zpl_json_object* f_property;
                f_property = zpl_json_find(&json_master_data, "files", false);

                if (!f_property)
                    return;

                for (i32 i = 0; i < zpl_array_count(f_property->nodes); ++i) {
                    zpl_json_object* file_node = (f_property->nodes + i);
                    zpl_json_object* file_property;

                    DlFile file;

                    file_property = zpl_json_find(file_node, "name", false);
                    file.name = std::string(baseURL + "/" + file_property->string);
                    file_property = zpl_json_find(file_node, "hash", false);
                    file.hash = std::string(file_property->string);

                    if (file.name.find("Models") != std::string::npos) {
                        CreateDirectory((fileDir + "\\Models").c_str(), NULL);
                    }
                    else if (file.name.find("Maps") != std::string::npos) {
                        CreateDirectory((fileDir + "\\Maps").c_str(), NULL);
                    }
                    else if (file.name.find("Missions") != std::string::npos) {
                        CreateDirectory((fileDir + "\\Missions").c_str(), NULL);
                    }
                    else if (file.name.find("Sounds") != std::string::npos) {
                        CreateDirectory((fileDir + "\\Sounds").c_str(), NULL);
                    }
                    else if (file.name.find("Anims") != std::string::npos) {
                        CreateDirectory((fileDir + "\\Anims").c_str(), NULL);
                    }

                    std::string path = file.name;
                    replaceAll(path, burl + "/", "");
                    replaceAll(path, "/", "\\");

                    std::string filePath = gpath + "\\mods\\" + folder + "\\" + path;
                    std::string hashPath = gpath + "\\mods\\" + folder + "\\" + path;
                    replaceAll(hashPath, "\\", "/");
                    std::string lPath = "mods\\" + folder + "\\" + path;

                    if (!fileExists(hashPath))
                    {
                        filesToDownload.push_back(file);
                    }
                    else
                    {
                        std::string fhash = get_hash(hashPath);
                        if (file.hash != fhash) filesToDownload.push_back(file);
                    }
                }
            }
        }

        printf("Files to download: %d\n", filesToDownload.size());
    }

    int dlProgress(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
    {
        percent = (NowDownloaded / TotalToDownload) * 100;
        downloaded = NowDownloaded;
        toDownload = TotalToDownload;
        return 0;
    }

    void processDl()
    {
        currentFileUrl = filesToDownload[i].name;

        std::string burl = baseURL;
        std::string path = currentFileUrl;
        std::string wpath = currentFileUrl;

        std::string gpath = GlobalConfig.gamepath;

        replaceAll(gpath, "//Game.exe", "");
        replaceAll(burl, "/files.txt", "");
        replaceAll(path, burl + "/", "");
        replaceAll(wpath, burl + "/", "");
        replaceAll(path, "/", "\\");
        replaceAll(gpath, "/", "\\");

        std::string filePath = gpath + "\\mods\\" + folder + "\\" + path;

        std::string fileName = filePath.substr(filePath.find_last_of("/\\") + 1);

        std::string fileDir = filePath;
        replaceAll(fileDir, fileName, "");

        printf("Processing download #%d: %s -> %s\n", i, currentFileUrl.c_str(), filePath.c_str());
        currentFileName = wpath;

        CURL* dl;
        FILE* f;
        CURLcode res;

        dl = curl_easy_init();

        if (dl)
        {
            CreateDirectory(fileDir.c_str(), NULL);

            f = fopen(filePath.c_str(), "wb");

            curl_easy_setopt(dl, CURLOPT_URL, currentFileUrl.c_str());
            curl_easy_setopt(dl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(dl, CURLOPT_WRITEDATA, f);
            curl_easy_setopt(dl, CURLOPT_NOPROGRESS, FALSE);
            curl_easy_setopt(dl, CURLOPT_PROGRESSFUNCTION, dlProgress);

            res = curl_easy_perform(dl);

            fclose(f);

            if (res == CURLE_OK)
            {
                printf("File downloaded successfully.\n");
            }
            else
            {
                isDownloading = false;
                printf("File failed to download.\n");
                modules::infobox::displayError("Cannot download file: " + wpath, NULL);
            }
            curl_easy_cleanup(dl);
        }
        else
        {
            printf("Cannot initialize CURL.\n");
            modules::infobox::displayError("Cannot initialize CURL", NULL);
        }
    }

    void startDownload()
    {
        createDlList();

        if (filesToDownload.size() >= 1)
        {
            input::block_input(true);

            isDownloading = true;

            switchClientState(ClientState_Downloading);

            for (int x = 0; x < filesToDownload.size(); x++)
            {
                i = x;
                if (!isDownloading)
                {
                    switchClientState(ClientState_Browser);
                    return;
                }
                processDl();
            }

            isDownloading = false;
        }

        server.needToDown = false;

        ServerInfo::join_server(server);
    }

    void init(ServerInfo::ServerData data)
    {
        server = data;

        baseURL = std::string(data.download_url);
        folder = std::string(data.download_id);

        http_t* request = http_get(baseURL.c_str(), NULL);

        if (!request)
        {
            printf("[Downloader] Invalid request.\n");
            modules::infobox::displayError("Cannot get download list!", NULL);
            jsonText = "{}";
        }
        else
        {
            f64 fetch_time = zpl_time_now();
            http_status_t status = HTTP_STATUS_PENDING;
            int prev_size = -1;
            while (status == HTTP_STATUS_PENDING)
            {
                status = http_process(request);
                if (prev_size != (int)request->response_size)
                {
                    prev_size = (int)request->response_size;
                }
                if (zpl_time_now() - fetch_time > MASTER_PULL_TIMEOUT)
                {
                    printf("[Downloader] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
                    modules::infobox::displayError("Cannot get download list!", NULL);
                    http_release(request);
                    jsonText = "{}";
                }
            }

            if (status == HTTP_STATUS_FAILED)
            {
                printf("[Downloader] HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
                modules::infobox::displayError("Cannot get download list!", NULL);
                http_release(request);
                jsonText = "{}";
            }

            jsonText = std::string(static_cast<const char*>(request->response_data));
            http_release(request);
            printf("[Downloader] Got the list!\n");
        }

        if (jsonText != "{}")
        {
            std::thread thr(startDownload);
            thr.detach();
        }
    }

    inline const char color_marker_start = '{';
    inline const char color_marker_end = '}';

    inline std::string remove_colors(const std::string text) {
        std::regex r("\\{(\\s*?.*?)*?\\}");

        return std::regex_replace(text, r, "");
    }

    inline bool process_inline_hex_color(const char* start, const char* end, ImVec4& color)
    {
        const int hexCount = (int)(end - start);
        if (hexCount == 6 || hexCount == 8)
        {
            char hex[9];
            strncpy(hex, start, hexCount);
            hex[hexCount] = 0;

            unsigned int hexColor = 0;
            if (sscanf(hex, "%x", &hexColor) > 0)
            {
                color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
                color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
                color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
                color.w = 1.0f;

                if (hexCount == 8)
                {
                    color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
                }

                return true;
            }
        }

        return false;
    }

    inline void draw_colored_text(const char* str)
    {
        char tempStr[4096];

        sprintf(tempStr, str);

        tempStr[sizeof(tempStr) - 1] = '\0';

        bool pushedColorStyle = false;
        const char* textStart = tempStr;
        const char* textCur = tempStr;
        while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
        {
            if (*textCur == color_marker_start)
            {
                // Print accumulated text
                if (textCur != textStart)
                {
                    ImGui::TextUnformatted(textStart, textCur);
                    ImGui::SameLine(0.0f, 0.0f);
                }

                // Process color code
                const char* colorStart = textCur + 1;
                do
                {
                    ++textCur;
                } while (*textCur != '\0' && *textCur != color_marker_end);

                // Change color
                if (pushedColorStyle)
                {
                    ImGui::PopStyleColor();
                    pushedColorStyle = false;
                }

                ImVec4 textColor;
                if (process_inline_hex_color(colorStart, textCur, textColor))
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                    pushedColorStyle = true;
                }

                textStart = textCur + 1;
            }
            else if (*textCur == '\n')
            {
                // Print accumulated text an go to next line
                ImGui::TextUnformatted(textStart, textCur);
                textStart = textCur + 1;
            }

            ++textCur;
        }

        if (textCur != textStart)
        {
            ImGui::TextUnformatted(textStart, textCur);
        }
        else
        {
            ImGui::NewLine();
        }

        if (pushedColorStyle)
        {
            ImGui::PopStyleColor();
        }
    }

    inline std::vector<std::string> Split(const std::string& str, int splitLength)
    {
        int NumSubstrings = str.length() / splitLength;
        std::vector<std::string> ret;

        for (auto i = 0; i < NumSubstrings; i++)
        {
            ret.push_back(str.substr(i * splitLength, splitLength));
        }

        // If there are leftover characters, create a shorter item at the end.
        if (str.length() % splitLength != 0)
        {
            ret.push_back(str.substr(splitLength * NumSubstrings));
        }


        return ret;
    }

    std::vector<std::string> Split(std::string str, std::string by)
    {
        std::vector<std::string> tokens;
        size_t prev = 0, pos = 0;
        do
        {
            pos = str.find(by, prev);
            if (pos == std::string::npos) pos = str.length();
            std::string token = str.substr(prev, pos - prev);
            if (!token.empty()) tokens.push_back(token);
            prev = pos + by.length();
        } while (pos < str.length() && prev < str.length());
        return tokens;
    }

    inline void formatted_colored_text(const char* fmt, ...) {
        char tempStr[4096];

        va_list argPtr;
        va_start(argPtr, fmt);
        _vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
        va_end(argPtr);

        int toSplit = 64;
        std::string buf = "";

        std::vector<std::string> colWords = Split(std::string(tempStr), " ");
        std::vector<std::string> words = Split(remove_colors(std::string(tempStr)), " ");
        std::vector<std::string> lines;

        int size = 0;

        for (int i = 0; i < words.size(); i++)
        {
            int strSize = words[i].size();
            if ((size + strSize) < toSplit)
            {
                size += strSize;
                buf += (colWords[i] + " ");
            }
            else
            {
                buf += (colWords[i] + "\n");
                size = 0;
            }
        }

        draw_colored_text(buf.c_str());
    }

    bool render()
    {
        ImGui::SetNextWindowPosCenter();
        ImGui::Begin("Downloading Files", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        {
            ImGui::Text(currentFileName.c_str());

            int progress = 0;

            if (percent < 0)
            {
                progress = 0;
            }
            else
            {
                progress = percent;
            }

            float dlKb = (float)downloaded / 1024.0f;
            float tKb = (float)toDownload / 1024.0f;
            float dlMb = dlKb / 1024.0f;
            float tMb = tKb / 1024.0f;

            if (dlKb >= 1024.0f)
            {
                ImGui::Text("%.2f MB / %.2f MB", dlMb, tMb);
            }
            else
            {
                if (tKb >= 1024.0f)
                {
                    ImGui::Text("%.2f KB / %.2f MB", dlKb, tMb);
                }
                else
                {
                    ImGui::Text("%.2f KB / %.2f KB", dlKb, tKb);
                }
            }
            

            ImGui::ProgressBar((float)progress / 100.0, ImVec2(-1, 0), std::string(std::to_string(progress) + "%").c_str());
            ImGui::Text("                                                                                                                                ");
        }
        ImGui::End();

        return true;
    }
}
