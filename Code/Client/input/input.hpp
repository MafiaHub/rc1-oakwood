#pragma once

struct CDirectInputDevice8Proxy;

namespace input {
    enum {
        ZINPUT_MOUSE,
        ZINPUT_KEYBOARD,
        ZINPUT_JOYSTICK,
    };

    struct InputState_ {
        CDirectInputDevice8Proxy* devices[4];
        bool input_blocked = false;
    } InputState;
}

namespace cef {
    void inject_winproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& pass, LRESULT& lresult);
}

#include "input/CDirectInputDevice8Proxy.h"
#include "input/CDirectInput8Proxy.h"

namespace input {

    typedef HRESULT(WINAPI *input_dxi8create_t)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
    input_dxi8create_t original_dxi8create  = nullptr;
    WNDPROC mod_wndproc_original_keyboard	= nullptr;
    WNDPROC mod_wndproc_original_mouse		= nullptr;

    HRESULT WINAPI input_dxi8create_hook(HINSTANCE hInst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter) {
        HRESULT hr = original_dxi8create(hInst, dwVersion, riidltf, ppvOut, punkOuter);
        if (SUCCEEDED(hr)) {
            IDirectInput8 *pInput = static_cast<IDirectInput8*>(*ppvOut);
            CDirectInput8Proxy *proxy = new CDirectInput8Proxy(pInput);
            proxy->AddRef();
            *ppvOut = proxy;
        }
        return hr;
    }

    /* 
    * Function wich gets messsages from both of windows
    * Used as main entrypoint for every messages transfered to input of GUI library
    */
    LRESULT wndproc_combined(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    
        //Process gui input only when our window is focues
        if(MafiaSDK::IsWindowFocused()) {
            LRESULT result;
            bool pass;
            cef::inject_winproc(hWnd, uMsg, wParam, lParam, pass, result);
            return result;
        }

        return false;
    }

    /* 
    * Function wich get winproc from keyboard window
    */
    LRESULT __stdcall mod_wndproc_hook_keyboard(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        if(wndproc_combined(hWnd, uMsg, wParam, lParam))
            return true;

        return CallWindowProc(mod_wndproc_original_keyboard, hWnd, uMsg, wParam, lParam);
    }

    /*
    * Function wich get winproc from mouse window
    */
    LRESULT __stdcall mod_wndproc_hook_mouse(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        if(wndproc_combined(hWnd, uMsg, wParam, lParam))
            return true;

        return CallWindowProc(mod_wndproc_original_mouse, hWnd, uMsg, wParam, lParam);
    }

    /*
    * Wait till dinput is loaded then hook
    */
    inline auto hook() {
        
        while (!GetModuleHandle("dinput8.dll")) {
            Sleep(100);
        }
        
        original_dxi8create = (input_dxi8create_t)(DetourFunction(DetourFindFunction((char*)"dinput8.dll", (char*)"DirectInput8Create"), (PBYTE)input_dxi8create_hook));
    }

    /* 
    * Function wich disable focus from all device :) 
    */
    inline auto block_input(bool do_block) -> void {

        if (do_block) {
            InputState.devices[ZINPUT_MOUSE]->masterAquired = false;
            InputState.devices[ZINPUT_MOUSE]->Unacquire();

            InputState.devices[ZINPUT_KEYBOARD]->masterAquired = false;
            InputState.devices[ZINPUT_KEYBOARD]->Unacquire();

            ShowCursor(TRUE);
        }
        else {
            InputState.devices[ZINPUT_MOUSE]->masterAquired = true;
            InputState.devices[ZINPUT_MOUSE]->Acquire();

            InputState.devices[ZINPUT_KEYBOARD]->masterAquired = true;
            InputState.devices[ZINPUT_KEYBOARD]->Acquire();

            ShowCursor(FALSE);
        }

        InputState.input_blocked = do_block;
    }

    /*
    * Hook both input windows winproc
    */
    inline auto hook_window() {

        MafiaSDK::GetIGraph()->SetAppName("Mafia Oakwood");

        auto mod_win32_hwnd_parent = (HWND)MafiaSDK::GetIGraph()->GetMainHWND();
        mod_wndproc_original_keyboard = (WNDPROC)SetWindowLongPtr(mod_win32_hwnd_parent, GWL_WNDPROC, (LONG_PTR)mod_wndproc_hook_keyboard);
        SetWindowLongW(mod_win32_hwnd_parent, GWL_WNDPROC, GetWindowLong(mod_win32_hwnd_parent, GWL_WNDPROC));
        
        auto mod_win32_hwnd = (HWND)MafiaSDK::GetIGraph()->GetChildHWND();
        mod_wndproc_original_mouse = (WNDPROC)SetWindowLongPtr(mod_win32_hwnd, GWL_WNDPROC, (LONG_PTR)mod_wndproc_hook_mouse);
        SetWindowLongW(mod_win32_hwnd, GWL_WNDPROC, GetWindowLong(mod_win32_hwnd, GWL_WNDPROC));
    }

    auto toggle_block_input() {
        block_input(!InputState.input_blocked);
    }
}
