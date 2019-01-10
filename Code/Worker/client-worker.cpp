#include <windows.h>
#include <delayimp.h>
#include <vector>
#include <mutex>

#define ZPL_IMPL
#include "librg/zpl.h"

double lastCoreUpdate = 0.0f;
bool isCoreUpdated = false;
std::mutex timeCheck;

#include <cef.hpp>
CefRefPtr<OakwoodCefApp> cefapp;

static LONG WINAPI TerminateInstantly(LPEXCEPTION_POINTERS pointers) {
    if (pointers->ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT) {
        TerminateProcess(GetCurrentProcess(), 0xDEADCAFE);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void shutdown() {
    AddVectoredExceptionHandler(FALSE, TerminateInstantly);
    CefShutdown();
    TerminateProcess(GetCurrentProcess(), 0);
}

int CALLBACK WinMain(HINSTANCE wininst, HINSTANCE pi, LPSTR cmdline, int nCmdShow) {
    // a hack to get current directory
    const char *prefix = "--log-file=\"";
    const char *suffix = "/cef/ceflog.txt";
    auto arglist = std::string(cmdline);
    auto start   = arglist.find(prefix) + strlen(prefix);
    auto total   = arglist.find_first_of("\"", start) - start - strlen(suffix);
    auto workdir = arglist.substr(start, total);

    SetCurrentDirectory((wchar_t *)zpl_utf8_to_ucs2_buf((u8 *)workdir.c_str()));
    auto addDllDirectory = (decltype(&AddDllDirectory))GetProcAddress(GetModuleHandle(L"kernel32.dll"), "AddDllDirectory");
    auto setDefaultDllDirectories = (decltype(&SetDefaultDllDirectories))GetProcAddress(GetModuleHandle(L"kernel32.dll"), "SetDefaultDllDirectories");

    if (addDllDirectory && setDefaultDllDirectories) {
        setDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);

        AddDllDirectory((wchar_t *)zpl_utf8_to_ucs2_buf((u8 *)workdir.c_str()));
        AddDllDirectory((wchar_t *)zpl_utf8_to_ucs2_buf((u8 *)(workdir + "/bin").c_str()));
    }

    __HrLoadAllImportsForDll("libcef.dll");
 
    CefMainArgs args(wininst);
    cefapp = new OakwoodCefApp;

    lastCoreUpdate = zpl_time_now();
    std::thread parentRunCheck([&]() {
        while (true) {
            timeCheck.lock();
            if (zpl_time_now() - lastCoreUpdate > 5.0) {
                if (isCoreUpdated) {
                    shutdown();
                    break;
                }
            }
            timeCheck.unlock();
            zpl_sleep_ms(100);
        }
    });

    parentRunCheck.detach();
    return CefExecuteProcess(args, cefapp, nullptr);
}
