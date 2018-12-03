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

int CALLBACK WinMain(HINSTANCE wininst, HINSTANCE pi, LPSTR cmdline, int nCmdShow) {
    __HrLoadAllImportsForDll("libcef.dll");
 
  //  SetCurrentDirectory(L"D:\\Projects\\zpl\\oakwood\\Bin\\Debug\\");
    CefMainArgs args(wininst);
    cefapp = new OakwoodCefApp;
   /*
    wchar_t Buffer[MAX_PATH];
    DWORD dwRet;
    
    
    dwRet = GetCurrentDirectory(MAX_PATH, Buffer);
    MessageBox(NULL, Buffer, L"foo", MB_OK);
    */
    /*
            auto addDllDirectory =
    (decltype(&AddDllDirectory))GetProcAddress(GetModuleHandle(L"kernel32.dll"), "AddDllDirectory");
    auto setDefaultDllDirectories =
    (decltype(&SetDefaultDllDirectories))GetProcAddress(GetModuleHandle(L"kernel32.dll"),
                                                        "SetDefaultDllDirectories");

    if (addDllDirectory && setDefaultDllDirectories) {
        setDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_USER_DIRS);

        AddDllDirectory(L"D:\\Projects\\zpl\\oakwood\\Bin\\Debug");
        AddDllDirectory(L"D:\\Projects\\zpl\\oakwood\\Bin\\Debug\\bin");
    }
*/

    lastCoreUpdate = zpl_time_now();
    std::thread parentRunCheck([&]() {
        while (true) {
            timeCheck.lock();
            if (zpl_time_now() - lastCoreUpdate > 5.0) {
                if (isCoreUpdated) {
                    CefShutdown();
                    exit(0);
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
