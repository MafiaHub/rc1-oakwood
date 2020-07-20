// General purpose - UI/Rendering manipulation using scripts

namespace script
{
    #include "imgui-wrapper.hpp"

    asIScriptEngine* engine;

    inline void msg_callback(const asSMessageInfo* msg, void* param)
    {
        const char* type = "ERROR";
        switch (msg->type)
        {
        case asMSGTYPE_WARNING:
            type = "WARNING";
            break;
        case asMSGTYPE_INFORMATION:
            type = "INFO";
            break;
        }

        if (msg->type == asMSGTYPE_ERROR)
        {
            chat::add_message("{ff0000}ERROR {ffffff}in {00ff00}\"" + std::string(msg->section) + "\" {acacac}({00ff00}" + std::to_string(msg->row) + "{ababab}, {00ff00}" + std::to_string(msg->col) + "{acacac}): {ffffff}" + std::string(msg->message));
        }

        printf("[ANGEL - %s] %s (%d, %d) %s", type, msg->section, msg->row, msg->col, msg->message);
    }

    int include_callback(const char* include, const char* from, CScriptBuilder* builder, void* userParam)
    {
        return builder->AddSectionFromFile(include);
    }

    void exception_callback(asIScriptContext* ctx)
    {
        asIScriptEngine* e = ctx->GetEngine();

        const asIScriptFunction* function = ctx->GetExceptionFunction();

        chat::add_message("{ff0000}RUNTIME ERROR: {ffffff}" + std::string(ctx->GetExceptionString()));
        chat::add_message("{ffffff}Function: {00ff00}" + std::string(function->GetDeclaration()));
        chat::add_message("{ffffff}}Module: {00ff00}" + std::string(function->GetModuleName()));
        chat::add_message("{ffffff}Section: {00ff00}" + std::string(function->GetScriptSectionName()));
        chat::add_message("{ffffff}Line: {00ff00}" + std::to_string(ctx->GetExceptionLineNumber()));
    }

    inline bool load_script(std::string file)
    {
        if (engine)
            engine->ShutDownAndRelease();

        engine = asCreateScriptEngine();

        int r = engine->SetMessageCallback(asFUNCTION(msg_callback), 0, asCALL_CDECL); if(r < 0) return false;

        RegisterStdString(engine);
        RegisterScriptAny(engine);
        RegisterScriptArray(engine, true);
        RegisterScriptDateTime(engine);
        RegisterScriptFile(engine);
        RegisterScriptFileSystem(engine);
        RegisterStdStringUtils(engine);

        CScriptBuilder builder;

        builder.SetIncludeCallback(include_callback, NULL);

        r = builder.StartNewModule(engine, "OakModule");
        if (r < 0)
        {
            chat::add_message("{ff0000}ERROR{ffffff}: Cannot start new script module.");
            return false;
        }
        r = builder.AddSectionFromFile(file.c_str());
        if (r < 0)
        {
            chat::add_message("{ff0000}ERROR{ffffff}: Failed to add script file.");
            return false;
        }
        r = builder.BuildModule();
        if (r < 0)
        {
            chat::add_message("{ff0000}ERROR{ffffff}: Failed to build script module, please fix all the issues and run again.");
            return false;
        }

        asIScriptModule* mod = engine->GetModule("OakModule");
        asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
        if (func == 0)
        {
            chat::add_message("{ff0000}ERROR{ffffff}: Entry point of script not found, please add 'void main()' into the script and run again.");
            return false;
        }

        // Create our context, prepare it, and then execute
        asIScriptContext* ctx = engine->CreateContext();
        ctx->SetExceptionCallback(asFUNCTION(exception_callback), nullptr, 1);
        ctx->Prepare(func);
        r = ctx->Execute();

        ctx->Release();

        return true;
    }
}
