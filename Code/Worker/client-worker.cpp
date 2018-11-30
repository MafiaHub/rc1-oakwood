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

    CefMainArgs args(wininst);
    cefapp = new OakwoodCefApp;

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
