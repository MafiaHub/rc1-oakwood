#pragma once
#include <vector>
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>

namespace nfx
{
    class UiApp : public
         CefApp,
         CefRenderProcessHandler,
         CefV8Handler
    {
        using V8EventHandler = std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>>;
        std::vector<V8EventHandler> eventhandlers;
    public:
        virtual ~UiApp() = default;

        void CallEvent(const CefV8ValueList &args) {
            for (const auto &handler : eventhandlers) {
                handler.second->ExecuteFunctionWithContext(handler.first, nullptr, args);
            }
        }

        bool Execute(const CefString& name,
            CefRefPtr<CefV8Value> object,
            const CefV8ValueList& arguments,
            CefRefPtr<CefV8Value>& retval,
            CefString& exception
        ) override
        {
            if (name == "addEventHandler") {
                if (arguments.size() != 1 || !arguments[0]->IsFunction())
                    return false;

                eventhandlers.emplace_back(CefV8Context::GetCurrentContext(), arguments[0]);
                return true;
            }

            if (name == "invoke") {
                if (arguments.size() != 2 || !arguments[0]->IsString() || !arguments[1]->IsString())
                    return false;

                auto msg = CefProcessMessage::Create("invoke");
                auto argList = msg->GetArgumentList();

                argList->SetSize(2);
                argList->SetString(0, arguments[0]->GetStringValue());
                argList->SetString(1, arguments[1]->GetStringValue());

                return CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(
                    PID_BROWSER,
                    msg
                );
            }

            return false;
        }

        void OnBeforeCommandLineProcessing(const CefString &, CefRefPtr<CefCommandLine> command_line) override {

            command_line->AppendSwitch("off-screen-rendering-enabled");
            command_line->AppendSwitch("transparent-painting-enabled");
            command_line->AppendSwitch("enable-experimental-web-platform-features");
            command_line->AppendSwitch("enable-media-stream");
            command_line->AppendSwitch("use-fake-ui-for-media-stream");
            command_line->AppendSwitch("enable-speech-input");
            command_line->AppendSwitch("ignore-gpu-blacklist");
            command_line->AppendSwitch("enable-usermedia-screen-capture");
            command_line->AppendSwitch("disable-direct-composition");
            command_line->AppendSwitchWithValue("default-encoding", "utf-8");
            command_line->AppendSwitch("disable-gpu-vsync");
            command_line->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
            command_line->AppendSwitch("force-gpu-rasterization");
        }

        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId, CefRefPtr<CefProcessMessage> message) OVERRIDE {
            
            if (message->GetName() == "callEvent") {
                auto args = message->GetArgumentList();

                CefV8ValueList arg_list;
                arg_list.push_back(CefV8Value::CreateString(args->GetString(0)));
                arg_list.push_back(CefV8Value::CreateString(args->GetString(1)));
                CallEvent(arg_list);
                return true;
            }

            if (message->GetName() == "coreUpdate") {

                timeCheck.lock();
                isCoreUpdated = true;
                lastCoreUpdate = zpl_time_now();
                timeCheck.unlock();
            }

            return false;
        }

        void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame>, CefRefPtr<CefV8Context> context) override {
            
            auto window = context->GetGlobal();
            window->SetValue("invoke", CefV8Value::CreateFunction("invoke", this), V8_PROPERTY_ATTRIBUTE_READONLY);
            window->SetValue("addEventHandler", CefV8Value::CreateFunction("addEventHandler", this), V8_PROPERTY_ATTRIBUTE_NONE);

            browser->SendProcessMessage(
                PID_BROWSER,
                CefProcessMessage::Create("register")
            );
        }

        void OnContextReleased(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, CefRefPtr<CefV8Context> context) override {
            if (eventhandlers.empty())
                return;

            auto it = eventhandlers.begin();
            while (it != eventhandlers.end()) {

                if (it->first->IsSame(context))
                    it = eventhandlers.erase(it);
                else
                    ++it;
            }
        }

        CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }
        IMPLEMENT_REFCOUNTING(UiApp);
    };
}
