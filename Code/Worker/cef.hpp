#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>

class OakwoodCefApp : public CefApp, CefRenderProcessHandler, CefV8Handler {
public:
    virtual ~OakwoodCefApp() = default;

    void OnBeforeCommandLineProcessing(const CefString &, CefRefPtr<CefCommandLine> cmd) override {
        cmd->AppendSwitch("disable-direct-composition");
        cmd->AppendSwitchWithValue("default-encoding", "utf-8");
        cmd->AppendSwitch("enable-experimental-web-platform-features");
        cmd->AppendSwitch("ignore-gpu-blacklist");
        cmd->AppendSwitch("enable-usermedia-screen-capture");
        cmd->AppendSwitch("transparent-painting-enabled");
        cmd->AppendSwitch("disable-gpu-vsync");
        cmd->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
        cmd->AppendSwitch("force-gpu-rasterization");
        cmd->AppendSwitch("enable-media-stream");
        cmd->AppendSwitch("use-fake-ui-for-media-stream");
        cmd->AppendSwitch("enable-speech-input");
        cmd->AppendSwitch("off-screen-rendering-enabled");

        // some GPUs are in the GPU blacklist as 'forcing D3D9'
        // this just forces D3D11 anyway.
        cmd->AppendSwitchWithValue("use-angle", "d3d11");
    }

    void OnContextReleased(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, CefRefPtr<CefV8Context> context) override {
        if (!mEventHandlers.empty()) {
            mEventHandlers.clear();
        }
    }

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame>, CefRefPtr<CefV8Context> context) override {
        context->GetGlobal()->SetValue("triggerNative", CefV8Value::CreateFunction("triggerNative", this), V8_PROPERTY_ATTRIBUTE_READONLY);
        context->GetGlobal()->SetValue("addNativeHandler", CefV8Value::CreateFunction("addNativeHandler", this), V8_PROPERTY_ATTRIBUTE_NONE);
    }

    bool Execute(const CefString& name,
        CefRefPtr<CefV8Value> object,
        const CefV8ValueList& arguments,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception
    ) override
    {
        if (name == "addNativeHandler") {
            if (arguments.size() != 1 || !arguments[0]->IsFunction()) {
                return false;
            }

            mEventHandlers.push_back(std::make_pair(CefV8Context::GetCurrentContext(), arguments[0]));
            return true;
        }

        if (name == "triggerNative") {
            if (arguments.size() != 2 || !arguments[0]->IsString() || !arguments[1]->IsString()) {
                return false;
            }

            auto msg = CefProcessMessage::Create("executeEvent");
            auto argList = msg->GetArgumentList(); {
                argList->SetSize(2);
                argList->SetString(0, arguments[0]->GetStringValue());
                argList->SetString(1, arguments[1]->GetStringValue());
            }

            return CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
        }

        return false;
    }

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId, CefRefPtr<CefProcessMessage> message) override {
        if (message->GetName() == "coreUpdate") {
            timeCheck.lock();
            isCoreUpdated = true;
            lastCoreUpdate = zpl_time_now();
            timeCheck.unlock();
        }

        if (message->GetName() == "executeEvent") {
            auto a = message->GetArgumentList();

            CefV8ValueList args; {
                args.push_back(CefV8Value::CreateString(a->GetString(0)));
                args.push_back(CefV8Value::CreateString(a->GetString(1)));
            }

            for (const auto &handler : mEventHandlers) {
                handler.second->ExecuteFunctionWithContext(handler.first, nullptr, args);
            }

            return true;
        }
        return false;
    }

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }
    std::vector<std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>>> mEventHandlers;
    IMPLEMENT_REFCOUNTING(OakwoodCefApp);
};
