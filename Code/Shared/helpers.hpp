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

std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}
