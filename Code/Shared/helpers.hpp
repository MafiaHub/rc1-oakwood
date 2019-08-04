#pragma once

#ifdef _WIN32
std::string get_platform_path() {
    char temp_path_raw[MAX_PATH] = { '\0' };
    GetModuleFileName(GetModuleHandle(NULL), temp_path_raw, MAX_PATH);
    auto temp_path = std::string(temp_path_raw);
    auto temp_pos = temp_path.rfind("\\");
    return temp_path.erase(temp_pos, std::string::npos);
}
#endif

