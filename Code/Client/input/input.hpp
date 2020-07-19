#pragma once

struct CDirectInputDevice8Proxy;
extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

namespace modules::player {
    inline void on_key_pressed(bool down, unsigned long key);
}

namespace input {
    enum {
        ZINPUT_MOUSE,
        ZINPUT_KEYBOARD,
        ZINPUT_JOYSTICK,
    };

    struct InputState_ {
        CDirectInputDevice8Proxy* devices[4];
        bool input_blocked = false;
        POINT mouse_move_delta;
        bool rmb_down = false;
        bool lmb_down = false;
    } InputState;

    std::unordered_map<unsigned long, bool> curent_key_states;

    inline bool is_key_down(DWORD key) {
        return curent_key_states[key];
    }

    class KeyToggle {
    public:
        KeyToggle(int key) :mKey(key), mActive(false) {}
        operator bool() {
            if (is_key_down(mKey)) {
                if (!mActive) {
                    mActive = true;
                    return true;
                }
            }
            else
                mActive = false;
            return false;
        }
    private:
        int mKey;
        bool mActive;
    };
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
    POINT last_mouse_pos;
    LRESULT wndproc_combined(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    
        if (uMsg == WM_SETTEXT) {
            printf("Window title changed: %s\n", lParam);
        }

        // Process gui input only when our window is focues
        if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
            curent_key_states[wParam] = true;
            modules::player::on_key_pressed(true, wParam);
        }
             
        if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) {
            curent_key_states[wParam] = false;
            modules::player::on_key_pressed(false, wParam);
        }

        if (uMsg == WM_MOUSEMOVE || uMsg == WM_MOUSELAST) {
            auto current_x = GET_X_LPARAM(lParam);
            auto current_y = GET_Y_LPARAM(lParam);

            InputState.mouse_move_delta.x = current_x - last_mouse_pos.x;
            InputState.mouse_move_delta.y = current_y - last_mouse_pos.y;

            last_mouse_pos = { current_x, current_y };
        }

        if (uMsg == WM_LBUTTONDOWN) {
            InputState.lmb_down = true;
        }
        else if (uMsg == WM_LBUTTONUP) {
            InputState.lmb_down = false;
        }

        if (uMsg == WM_RBUTTONDOWN) {
            InputState.rmb_down = true;
        }
        else if (uMsg == WM_RBUTTONUP) {
            InputState.rmb_down = false;
        }

        return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    }

    /* 
    * Function wich get winproc from keyboard window
    */
    LRESULT __stdcall mod_wndproc_hook_keyboard(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        wndproc_combined(hWnd, uMsg, wParam, lParam);
        return CallWindowProc(mod_wndproc_original_keyboard, hWnd, uMsg, wParam, lParam);
    }

    /*
    * Function wich get winproc from mouse window
    */
    LRESULT __stdcall mod_wndproc_hook_mouse(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        wndproc_combined(hWnd, uMsg, wParam, lParam);
        return CallWindowProc(mod_wndproc_original_mouse, hWnd, uMsg, wParam, lParam);
    }

    /*
    * Wait till dinput is loaded then hook
    */
    inline auto hook() {
        original_dxi8create = (input_dxi8create_t)(DetourFunction(DetourFindFunction((char*)"dinput8.dll", (char*)"DirectInput8Create"), (PBYTE)input_dxi8create_hook));
    }

    /* 
    * Function wich disable focus from all device :) 
    */
    inline auto block_input(bool do_block) -> void {

        if (do_block) {
            InputState.devices[ZINPUT_MOUSE]->masterAquired = false;
            InputState.devices[ZINPUT_MOUSE]->Unacquire();
            while (ShowCursor(TRUE) < 0) {}
        } else {
            InputState.devices[ZINPUT_MOUSE]->masterAquired = true;
            InputState.devices[ZINPUT_MOUSE]->Acquire();
            while (ShowCursor(FALSE) >= 0) {}
        }

        InputState.input_blocked = do_block;
    }

    /*
    * Hook both input windows winproc
    */
    inline auto hook_window() {
        auto mod_win32_hwnd_parent = (HWND)MafiaSDK::GetIGraph()->GetMainHWND();
        mod_wndproc_original_keyboard = (WNDPROC)SetWindowLongPtr(mod_win32_hwnd_parent, GWL_WNDPROC, (LONG_PTR)mod_wndproc_hook_keyboard);
        SetWindowLongW(mod_win32_hwnd_parent, GWL_WNDPROC, GetWindowLong(mod_win32_hwnd_parent, GWL_WNDPROC));
        
        auto mod_win32_hwnd = (HWND)MafiaSDK::GetIGraph()->GetChildHWND();
        mod_wndproc_original_mouse = (WNDPROC)SetWindowLongPtr(mod_win32_hwnd, GWL_WNDPROC, (LONG_PTR)mod_wndproc_hook_mouse);
        SetWindowLongW(mod_win32_hwnd, GWL_WNDPROC, GetWindowLong(mod_win32_hwnd, GWL_WNDPROC));

        //SetWindowTextA(mod_win32_hwnd_parent, "Mafia: Oakwood");
    }

    auto toggle_block_input() {
        block_input(!InputState.input_blocked);
    }
}
