#ifndef _WIN32
int main()
#else

#include <windows.h>
#include <delayimp.h>
#include <mutex>

#define ZPL_IMPL
#include "librg/zpl.h"

double lastCoreUpdate = 0.0f;
bool isCoreUpdated = false;
std::mutex timeCheck;

#include <ui/uiapp.h>

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
#endif
{
#ifdef _WIN32

    __HrLoadAllImportsForDll("libcef.dll");

    const CefMainArgs args(hInstance);
#else
    const CefMainArgs args();
#endif

    CefRefPtr<nfx::UiApp> ui_app = new nfx::UiApp;
    
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

    return CefExecuteProcess(args, ui_app, nullptr);
}
