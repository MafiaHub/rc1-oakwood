#pragma once
#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"
#include <windowsx.h>

namespace cef {

    class OakwoodRenderHandler;
    class OakwoodBrowserClient;
    class CefMinimal;

    // =======================================================================//
    // !
    // ! CEF structs & globals
    // !
    // =======================================================================//

    struct object {
        u8 type;
        u8 visible : 1;
        u8 queued : 1;
        u8 metadata : 6;
        CefRefPtr<CefBrowser> browser;
        CefRefPtr<OakwoodBrowserClient> client;
        CefRefPtr<OakwoodRenderHandler> renderer;
    };

    f64 last_core_update = 0.0f;
    CefRefPtr<CefMinimal> minimal;
    ID3DXSprite* rendering_sprite = nullptr;
    std::vector<object*> browsers;

    using native_function = std::function<void(CefRefPtr<CefListValue>)>;
    std::unordered_map<std::string, native_function> native_functions;

    void register_native(std::string function_name, native_function function_ptr) {
        native_functions[function_name] = function_ptr;
    }


    // =======================================================================//
    // !
    // ! CEF interfaces
    // !
    // =======================================================================//

    class OakwoodRenderHandler : public CefRenderHandler {
    public:
        unsigned char* mPixelBuffer;
        int mPixelBufferWidth;
        int mPixelBufferHeight;
        unsigned char* mPopupBuffer;
        unsigned char* mPixelBufferRow;
        CefRect mPopupBufferRect;
        int mBufferDepth;
        bool mFlipYPixels;
        std::mutex mTextureMutex;
        IDirect3DTexture9* mTexture;

        OakwoodRenderHandler(IDirect3DDevice9* device, int w, int h, int zindex) {

            mFlipYPixels = false;
            mBufferDepth = 4;
            mPixelBuffer = (unsigned char*)malloc(w * h * mBufferDepth);
            mPixelBufferWidth = w;
            mPixelBufferHeight = h;
            mPopupBuffer = nullptr;
            mPixelBufferRow = nullptr;

            InitTexture(device);
        }

        ~OakwoodRenderHandler() {
            delete[] mPixelBuffer;
            delete[] mPopupBuffer;
            delete[] mPixelBufferRow;
            delete[] mTexture;
        }

        void copyPopupIntoView() {
            int popup_y = (mFlipYPixels ? (mPixelBufferHeight - mPopupBufferRect.y) : mPopupBufferRect.y);
            unsigned char* src = (unsigned char*)mPopupBuffer;
            unsigned char* dst = mPixelBuffer + popup_y * mPixelBufferWidth * mBufferDepth + mPopupBufferRect.x * mBufferDepth;
            while (src < (unsigned char*)mPopupBuffer + mPopupBufferRect.width * mPopupBufferRect.height * mBufferDepth) {
                memcpy(dst, src, mPopupBufferRect.width * mBufferDepth);
                src += mPopupBufferRect.width * mBufferDepth;
                dst += mPixelBufferWidth * mBufferDepth * (mFlipYPixels ? -1 : 1);
            }
        }

        bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override {
            rect = CefRect(0, 0, mPixelBufferWidth, mPixelBufferHeight);
            return true;
        }

        void OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info) override {
            SetClassLong((HWND)MafiaSDK::GetMainWindow(), GCL_HCURSOR, (LONG)cursor);
        }

        void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override {
            CEF_REQUIRE_UI_THREAD();

            if (type == PET_VIEW) {
                memcpy(mPixelBuffer, buffer, width * height * mBufferDepth);

                if (mFlipYPixels) {
                    const size_t stride = mPixelBufferWidth * mBufferDepth;
                    unsigned char* lower = mPixelBuffer;
                    unsigned char* upper = mPixelBuffer + (mPixelBufferHeight - 1) * stride;
                    while (lower < upper) {
                        memcpy(mPixelBufferRow, lower, stride);
                        memcpy(lower, upper, stride);
                        memcpy(upper, mPixelBufferRow, stride);
                        lower += stride;
                        upper -= stride;
                    }
                }

                if (mPopupBuffer != nullptr) {
                    copyPopupIntoView();
                }
            }

            else if (type == PET_POPUP) {
                memcpy(mPopupBuffer, buffer, width * height * mBufferDepth);
                copyPopupIntoView();
            }

            if (mPixelBufferWidth > 0 && mPixelBufferHeight > 0) {

                D3DLOCKED_RECT rect;
                mTextureMutex.lock();
                if (mTexture) {
                    mTexture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);

                    unsigned char *pDestPixels = (unsigned char*)rect.pBits;
                    unsigned char *pSourcePixels = (unsigned char *)mPixelBuffer;

                    for (int y = 0; y < mPixelBufferHeight; ++y) {
                        memcpy(pDestPixels, pSourcePixels, mPixelBufferWidth * 4);
                        pSourcePixels += rect.Pitch;
                        pDestPixels += rect.Pitch;
                    }

                    mTexture->UnlockRect(NULL);
                }
                mTextureMutex.unlock();
            }
        }

        void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override {
            CEF_REQUIRE_UI_THREAD();

            if (!show) {
                delete[] mPopupBuffer;
                mPopupBuffer = nullptr;
                mPopupBufferRect.Reset();
            }
        }

        void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override {
            CEF_REQUIRE_UI_THREAD();

            mPopupBufferRect = rect;
            if (mPopupBuffer == nullptr) {
                mPopupBuffer = new unsigned char[rect.width * rect.height * mBufferDepth];
                memset(mPopupBuffer, 0xff, rect.width * rect.height * mBufferDepth);
            }
        }

        void GetTexture(std::function<void(IDirect3DTexture9*)> fPtr) {
            mTextureMutex.lock();
            fPtr(mTexture);
            mTextureMutex.unlock();
        }

        void InitTexture(IDirect3DDevice9* device) {
            mTextureMutex.lock();
            if (FAILED(D3DXCreateTexture(device, mPixelBufferWidth, mPixelBufferHeight, NULL, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mTexture))) {
                MessageBox(NULL, "Failed to create the texture for web-view", "Error CEF M2ORenderHandler Initialization", MB_OK);
                return;
            }
            mTextureMutex.unlock();
        }

        IMPLEMENT_REFCOUNTING(OakwoodRenderHandler);
    };

    class OakwoodBrowserClient : public CefClient, public CefLifeSpanHandler {
    public:
        OakwoodBrowserClient(OakwoodRenderHandler* handler) :
            mRenderHandler(handler) {}

        void OnAfterCreated(CefRefPtr<CefBrowser> browser) {
            CEF_REQUIRE_UI_THREAD();
            mBrowserId = browser->GetIdentifier();
        }

        bool DoClose(CefRefPtr<CefBrowser> browser) {
            CEF_REQUIRE_UI_THREAD();

            if (browser->GetIdentifier() == mBrowserId) {
                mClosing = true;
            }

            return false;
        }

        virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser>, CefProcessId, CefRefPtr<CefProcessMessage> msg) override {
            if (msg->GetName() == "executeEvent") {
                auto args = msg->GetArgumentList();
                for (auto native : native_functions) {
                    if (native.first.find(args->GetString(0).ToString()) != std::string::npos) {
                        native.second(args);
                    }
                }
                return true;
            }

            return false;
        }

        bool IsClosing() { return mClosing; }

        CefRefPtr<CefRenderHandler> GetRenderHandler() override {
            return mRenderHandler;
        }

        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
            return this;
        }

        IMPLEMENT_REFCOUNTING(OakwoodBrowserClient);
    private:
        int mBrowserId;
        bool mClosing;
        CefRefPtr<CefRenderHandler> mRenderHandler;
    };

    class CefMinimal : public CefApp {
    public:
        virtual ~CefMinimal() = default;
        void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override {
            command_line->AppendSwitch("enable-begin-frame-scheduling");
            command_line->AppendSwitch("force-gpu-rasterization");
            command_line->AppendSwitchWithValue("disable-features", "TouchpadAndWheelScrollLatching");
            command_line->AppendSwitch("disable-smooth-scrolling");
            command_line->AppendSwitchWithValue("disable-features", "AsyncWheelEvents");
            command_line->AppendSwitch("disable-direct-composition");
            command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
            command_line->AppendSwitch("enable-experimental-web-platform-features");
            command_line->AppendSwitch("transparent-painting-enabled");
            command_line->AppendSwitch("off-screen-rendering-enabled");
        }

        IMPLEMENT_REFCOUNTING(CefMinimal);
    };

    // =======================================================================//
    // !
    // ! Internal CEF implementation
    // !
    // =======================================================================//
    int init(IDirect3DDevice9* device) {

        minimal = new CefMinimal();

        CefSettings settings;
        CefMainArgs args(GetModuleHandle(nullptr));

        std::string path = get_platform_path();
        CefString(&settings.resources_dir_path) = path;
        CefString(&settings.log_file) = path + "\\cef\\ceflog.txt";
        CefString(&settings.locales_dir_path) = path + "\\cef\\locales";
        CefString(&settings.cache_path) = path + "\\cef\\cache";
        CefString(&settings.user_data_path) = path + "\\cef\\userdata";
        CefString(&settings.browser_subprocess_path) = path + "\\OakwoodWorker.exe";

        settings.multi_threaded_message_loop = false;
        settings.log_severity = LOGSEVERITY_WARNING;
        settings.remote_debugging_port = 7777;
        settings.windowless_rendering_enabled = true;
        settings.no_sandbox = true;

        if (!CefInitialize(args, settings, minimal, nullptr)) {
            printf("CEF [Error] unable to initalize cef...\n");
            return -1;
        }

        if (FAILED(D3DXCreateSprite(device, &rendering_sprite))) {
            MessageBox(NULL, "Failed to create the sprite", "Error init Initialization", MB_OK);
            return -1;
        }

        return -1;
    }

    int tick() {
        CefDoMessageLoopWork();

        if (zpl_time_now() - last_core_update > 3.0) {
            for (auto browser : browsers) {
                if (browser) {
                    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("coreUpdate");
                    browser->browser.get()->SendProcessMessage(PID_RENDERER, msg);
                }
            }
            last_core_update = zpl_time_now();
        }

        return 0;
    }

    void browser_destroy(object* obj);
    int free() {

        for (auto browser : browsers) {
            browser_destroy(browser);
        }

        browsers.clear();
        CefShutdown();
        return 0;
    }

    // =======================================================================//
    // !
    // ! Resource creation/destruction
    // !
    // =======================================================================//

    object* browser_create(IDirect3DDevice9* device, const char *url, int w, int h, int zindex) {

        object* new_browser = new object;
        HWND win_id = (HWND)MafiaSDK::GetMainWindow();

        CefWindowInfo window_info;
        window_info.SetAsWindowless(NULL);

        CefBrowserSettings settings;
        settings.windowless_frame_rate = 60;

        CefString cefurl(url);
        new_browser->type = 0;
        new_browser->visible = 1;
        new_browser->renderer = new OakwoodRenderHandler(device, w, h, zindex);
        new_browser->client = new OakwoodBrowserClient(new_browser->renderer);
        new_browser->browser = CefBrowserHost::CreateBrowserSync(window_info, new_browser->client, cefurl, settings, nullptr);
        new_browser->browser->GetHost()->SetFocus(true);
        browsers.push_back(new_browser);

        return new_browser;
    }

    void device_lost() {
        
        if (rendering_sprite) {
            rendering_sprite->Release();
            rendering_sprite = nullptr;
        }

        if (!browsers.empty()) {
            for (auto handle : browsers) {
                if (handle && handle->renderer) {

                    handle->renderer->GetTexture([](auto texture) {
                        if (texture) {
                            texture->Release();
                            texture = nullptr;
                        }
                    });
                }
            }
        }
    }

    void device_reset(IDirect3DDevice9* device) {
        
        if (FAILED(D3DXCreateSprite(device, &rendering_sprite))) {
            MessageBox(NULL, "Failed to create the sprite", "Error init Initialization", MB_OK);
            return;
        }

        if (!browsers.empty()) {
            for (auto handle : browsers) {
                if (handle) {
                    handle->renderer->InitTexture(device);
                }
            }
        }
    }

    void render_browsers() {

        if (!browsers.empty() && rendering_sprite != nullptr) {
            rendering_sprite->Begin(D3DXSPRITE_ALPHABLEND);
            for (auto handle : browsers) {

                if (handle->visible) {
                    
                    handle->renderer->GetTexture([=](auto texture) {
                        if (texture) {
                            rendering_sprite->Draw(texture, NULL, NULL, NULL, 0xFFFFFFFF);
                        }
                    });
                }
            }
            rendering_sprite->End();
        }
    }

    void browser_destroy(object* obj) {

        if (obj) {
            obj->visible = 0;
            obj->browser.get()->GetHost()->CloseBrowser(true);
            obj->browser = NULL;
            obj->client = NULL;
            obj->renderer = NULL;

            auto to_remove = std::find(browsers.begin(), browsers.end(), obj);
            browsers.erase(to_remove);
        }
    }

    // =======================================================================//
    // !
    // ! Operation interface
    // !
    // =======================================================================//

    int browser_show(object* handle, bool show) {

        if (handle) {
            handle->visible = show;

            if (handle->visible) {
                handle->browser.get()->GetHost()->SetFocus(true);
            }
        }

        return 0;
    }

    // =======================================================================//
    // !
    // ! Location hanlding
    // !
    // =======================================================================//

    void browser_reload(object* handle) {

        if (handle) {
            handle->browser.get()->Reload();
        }
    }

    void url_set(object* handle, const char *url) {

        if (handle) {
            CefString cefurl(url);
            handle->browser.get()->GetMainFrame()->LoadURL(cefurl);
        }
    }

    // =======================================================================//
    // !
    // ! Input handling
    // !
    // =======================================================================//

    bool IsKeyDown(WPARAM wparam) {
        return (GetKeyState(wparam) & 0x8000) != 0;
    }

    int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam)
    {
        int modifiers = 0;
        if (IsKeyDown(VK_SHIFT))
            modifiers |= EVENTFLAG_SHIFT_DOWN;
        if (IsKeyDown(VK_CONTROL))
            modifiers |= EVENTFLAG_CONTROL_DOWN;
        if (IsKeyDown(VK_MENU))
            modifiers |= EVENTFLAG_ALT_DOWN;

        // Low bit set from GetKeyState indicates "toggled".
        if (::GetKeyState(VK_NUMLOCK) & 1)
            modifiers |= EVENTFLAG_NUM_LOCK_ON;
        if (::GetKeyState(VK_CAPITAL) & 1)
            modifiers |= EVENTFLAG_CAPS_LOCK_ON;

        switch (wparam)
        {
        case VK_RETURN:
            if ((lparam >> 16) & KF_EXTENDED)
                modifiers |= EVENTFLAG_IS_KEY_PAD;
            break;
        case VK_INSERT:
        case VK_DELETE:
        case VK_HOME:
        case VK_END:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
            if (!((lparam >> 16) & KF_EXTENDED))
                modifiers |= EVENTFLAG_IS_KEY_PAD;
            break;
        case VK_NUMLOCK:
        case VK_NUMPAD0:
        case VK_NUMPAD1:
        case VK_NUMPAD2:
        case VK_NUMPAD3:
        case VK_NUMPAD4:
        case VK_NUMPAD5:
        case VK_NUMPAD6:
        case VK_NUMPAD7:
        case VK_NUMPAD8:
        case VK_NUMPAD9:
        case VK_DIVIDE:
        case VK_MULTIPLY:
        case VK_SUBTRACT:
        case VK_ADD:
        case VK_DECIMAL:
        case VK_CLEAR:
            modifiers |= EVENTFLAG_IS_KEY_PAD;
            break;
        case VK_SHIFT:
            if (IsKeyDown(VK_LSHIFT))
                modifiers |= EVENTFLAG_IS_LEFT;
            else if (IsKeyDown(VK_RSHIFT))
                modifiers |= EVENTFLAG_IS_RIGHT;
            break;
        case VK_CONTROL:
            if (IsKeyDown(VK_LCONTROL))
                modifiers |= EVENTFLAG_IS_LEFT;
            else if (IsKeyDown(VK_RCONTROL))
                modifiers |= EVENTFLAG_IS_RIGHT;
            break;
        case VK_MENU:
            if (IsKeyDown(VK_LMENU))
                modifiers |= EVENTFLAG_IS_LEFT;
            else if (IsKeyDown(VK_RMENU))
                modifiers |= EVENTFLAG_IS_RIGHT;
            break;
        case VK_LWIN:
            modifiers |= EVENTFLAG_IS_LEFT;
            break;
        case VK_RWIN:
            modifiers |= EVENTFLAG_IS_RIGHT;
            break;
        }
        return modifiers;
    }

    int GetCefMouseModifiers(WPARAM wparam) {

        int modifiers = 0;
        if (wparam & MK_CONTROL)
            modifiers |= EVENTFLAG_CONTROL_DOWN;
        if (wparam & MK_SHIFT)
            modifiers |= EVENTFLAG_SHIFT_DOWN;
        if (IsKeyDown(VK_MENU))
            modifiers |= EVENTFLAG_ALT_DOWN;
        if (wparam & MK_LBUTTON)
            modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
        if (wparam & MK_MBUTTON)
            modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
        if (wparam & MK_RBUTTON)
            modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

        // Low bit set from GetKeyState indicates "toggled".
        if (::GetKeyState(VK_NUMLOCK) & 1)
            modifiers |= EVENTFLAG_NUM_LOCK_ON;
        if (::GetKeyState(VK_CAPITAL) & 1)
            modifiers |= EVENTFLAG_CAPS_LOCK_ON;
        return modifiers;
    }

    void inject_winproc_one(CefRefPtr<CefBrowser> browser, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& pass, LRESULT& lresult);
    void inject_winproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& pass, LRESULT& lresult) {

        LRESULT result;
        for (auto browser : browsers) {
            if (browser && browser->visible) {
                inject_winproc_one(browser->browser.get(), hWnd, msg, wParam, lParam, pass, result);
            }
        }
    }

    void inject_winproc_one(CefRefPtr<CefBrowser> browser, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& pass, LRESULT& lresult) {

        static bool g_lastFocus = false;
        static int lastClickX;
        static int lastClickY;
        static int lastClickCount;
        static LONG lastClickTime;
        static CefBrowserHost::MouseButtonType lastClickButton;
        static bool mouseTracking;

        LONG currentTime = 0;
        bool cancelPreviousClick = false;

        if (msg == WM_CLOSE || msg == WM_DESTROY) {
            free();
            return;
        }

        if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN ||
            msg == WM_MBUTTONDOWN || msg == WM_MOUSEMOVE ||
            msg == WM_MOUSELEAVE) {
            currentTime = GetMessageTime();
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            cancelPreviousClick =
                (abs(lastClickX - x) > (GetSystemMetrics(SM_CXDOUBLECLK) / 2))
                || (abs(lastClickY - y) > (GetSystemMetrics(SM_CYDOUBLECLK) / 2))
                || ((currentTime - lastClickTime) > GetDoubleClickTime());
            if (cancelPreviousClick &&
                (msg == WM_MOUSEMOVE || msg == WM_MOUSELEAVE)) {
                lastClickCount = 0;
                lastClickX = 0;
                lastClickY = 0;
                lastClickTime = 0;
            }
        }

        switch (msg)
        {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            CefBrowserHost::MouseButtonType btnType =
                ((msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) ? MBT_LEFT : (
                (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) ? MBT_RIGHT : MBT_MIDDLE));
            if (!cancelPreviousClick && (btnType == lastClickButton)) {
                ++lastClickCount;
            }
            else {
                lastClickCount = 1;
                lastClickX = x;
                lastClickY = y;
            }
            lastClickTime = currentTime;
            lastClickButton = btnType;

            CefMouseEvent mouse_event;
            mouse_event.x = x;
            mouse_event.y = y;
            mouse_event.modifiers = GetCefMouseModifiers(wParam);

            if (browser)
            {
                browser->GetHost()->SendMouseClickEvent(mouse_event, btnType, false, lastClickCount);
            }

            pass = false;
            lresult = FALSE;
        } break;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            CefBrowserHost::MouseButtonType btnType =
                (msg == WM_LBUTTONUP ? MBT_LEFT : (
                    msg == WM_RBUTTONUP ? MBT_RIGHT : MBT_MIDDLE));

            if (browser)
            {
                CefMouseEvent mouse_event;
                mouse_event.x = x;
                mouse_event.y = y;
                mouse_event.modifiers = GetCefMouseModifiers(wParam);
                browser->GetHost()->SendMouseClickEvent(mouse_event, btnType, true,
                    lastClickCount);
            }

            pass = false;
            lresult = FALSE;
            break;
        }
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (!mouseTracking) {
                // Start tracking mouse leave. Required for the WM_MOUSELEAVE event to
                // be generated.
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;
                TrackMouseEvent(&tme);
                mouseTracking = true;
            }

            if (browser)
            {
                CefMouseEvent mouse_event;
                mouse_event.x = x;
                mouse_event.y = y;
                mouse_event.modifiers = GetCefMouseModifiers(wParam);
                browser->GetHost()->SendMouseMoveEvent(mouse_event, false);
            }

            pass = false;
            lresult = FALSE;
            break;
        }

        case WM_MOUSELEAVE: {
            if (mouseTracking) {
                // Stop tracking mouse leave.
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE & TME_CANCEL;
                tme.hwndTrack = hWnd;
                TrackMouseEvent(&tme);
                mouseTracking = false;
            }

            if (browser) {
                // Determine the cursor position in screen coordinates.
                POINT p;
                ::GetCursorPos(&p);
                ::ScreenToClient(hWnd, &p);

                CefMouseEvent mouse_event;
                mouse_event.x = p.x;
                mouse_event.y = p.y;
                mouse_event.modifiers = GetCefMouseModifiers(wParam);
                browser->GetHost()->SendMouseMoveEvent(mouse_event, true);
            }

            pass = false;
            lresult = FALSE;
        } break;

        case WM_MOUSEWHEEL: {

            if (browser) {
                POINT screen_point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                HWND scrolled_wnd = ::WindowFromPoint(screen_point);
                if (scrolled_wnd != hWnd)
                    break;

                ScreenToClient(hWnd, &screen_point);
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);

                CefMouseEvent mouse_event;
                mouse_event.x = screen_point.x;
                mouse_event.y = screen_point.y;
                mouse_event.modifiers = GetCefMouseModifiers(wParam);

                browser->GetHost()->SendMouseWheelEvent(mouse_event,
                    IsKeyDown(VK_SHIFT) ? delta : 0,
                    !IsKeyDown(VK_SHIFT) ? delta : 0);
            }

            pass = false;
            lresult = FALSE;
            break;
        }
        }

        if (msg == WM_KEYUP || msg == WM_KEYDOWN || msg == WM_CHAR) {
            CefKeyEvent keyEvent;

            keyEvent.windows_key_code = wParam;
            keyEvent.native_key_code = lParam;
            keyEvent.modifiers = GetCefKeyboardModifiers(wParam, lParam);

            if (msg != WM_CHAR) {
                keyEvent.type = (msg == WM_KEYDOWN) ? KEYEVENT_RAWKEYDOWN : KEYEVENT_KEYUP;
            }
            else {
                keyEvent.type = KEYEVENT_CHAR;
            }

            if (browser) {
                browser->GetHost()->SendKeyEvent(keyEvent);
            }

            pass = false;
            lresult = FALSE;
            return;
        }
        else if (msg == WM_INPUT && input::InputState.input_blocked) {
            pass = false;
            lresult = TRUE;
            return;
        }
        else if (msg == WM_IME_SETCONTEXT) {
            // We handle the IME Composition Window ourselves (but let the IME Candidates
            // Window be handled by IME through DefWindowProc()), so clear the
            // ISC_SHOWUICOMPOSITIONWINDOW flag:
            lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
            ::DefWindowProc(hWnd, msg, wParam, lParam);
            pass = false;
            lresult = false;
            return;
        }
        else if (msg == WM_IME_KEYLAST || msg == WM_IME_KEYDOWN || msg == WM_IME_KEYUP) {
            pass = false;
            lresult = false;
            return;
        }
    }
};
