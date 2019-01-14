#pragma once
namespace crashhandler
{
    bool exceptions_capturing = true;

    /*
    * Method for posting crash report to the server
    * that creates new process for sending data 
    */
    inline PROCESS_INFORMATION launch_process(std::string app, std::string arg) {
        STARTUPINFOW si;
        PROCESS_INFORMATION pi; // The function returns this
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        //Prepare CreateProcess args
        std::wstring app_w(app.length(), L' '); // Make room for characters
        std::copy(app.begin(), app.end(), app_w.begin()); // Copy string to wstring.

        std::wstring arg_w(arg.length(), L' '); // Make room for characters
        std::copy(arg.begin(), arg.end(), arg_w.begin()); // Copy string to wstring.

        std::wstring input = app_w + L" " + arg_w;
        wchar_t* arg_concat = const_cast<wchar_t*>(input.c_str());
        const wchar_t* app_const = app_w.c_str();

        // Start the child process.
        if (!CreateProcessW(
            NULL,      // app path
            arg_concat,     // Command line (needs to include app path as first argument. args seperated by whitepace)
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &pi)           // Pointer to PROCESS_INFORMATION structure
            )
        {
            printf("CreateProcess failed (%d).\n", GetLastError());
            throw std::exception("Could not create child process");
        }
        else
        {
            std::cout << "[] Successfully launched child process" << std::endl;
        }
        return pi;
    }

    inline void send_report(char* report_buffer) {

        std::string starting_params = "\"" + std::string(GlobalConfig.username) + "|" + std::string(GlobalConfig.server_address) + "|" + std::string(report_buffer) + "\"";
        auto stuff = launch_process("bin\\oakreport.exe", starting_params);
        ResumeThread(stuff.hThread);
    }

    std::stringstream buffer_to_send;
    class StackWalkerToSend : public StackWalker
    {
    public:
        StackWalkerToSend() {
            StackWalker(StackWalkOptions::RetrieveSymbol | StackWalkOptions::RetrieveLine);
        }
    protected:
        virtual void OnOutput(LPCSTR text) { 
            buffer_to_send << std::string(text);
        }
    };


    /*
    * Gets address of exception and find corresponding module 
    */
    inline void GetAdressAsModule(DWORD addr, char* outbuffer) {
        HMODULE hMods[512];
        HANDLE hProcess = GetCurrentProcess();
        DWORD cbNeeded;
        unsigned int i;
        DWORD close = 0x0;
        if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
            for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                if (addr - (DWORD)hMods[i] < addr - close)
                    close = (DWORD)hMods[i];
            }

            if (close != NULL) {
                TCHAR szModName[MAX_PATH];
                if (GetModuleBaseName(hProcess, (HMODULE)close, szModName,
                    sizeof(szModName) / sizeof(TCHAR))) {
                    sprintf(outbuffer, "%s+0x%p", szModName, (addr - close));
                    return;
                }
            }
            else {
                sprintf(outbuffer, "UNKNOWN");
            }
        }
    }

    /* 
    * Main exception filter for crashes
    */
    LONG WINAPI CrashExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo) {
        
        if (!exceptions_capturing) 
            return EXCEPTION_EXECUTE_HANDLER;

        if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
            return EXCEPTION_CONTINUE_EXECUTION;

        StackWalkerToSend sw;
        sw.ShowCallstack(GetCurrentThread(), ExceptionInfo->ContextRecord);

        send_report((char*)buffer_to_send.str().c_str());
        FILE* log = fopen("crashdump.txt", "a");
        if (log) {
            fprintf(log, "---\nMINIDUMP\n%s\n", buffer_to_send.str().c_str());
            fclose(log);
        }
        
        return EXCEPTION_EXECUTE_HANDLER;
    }

    /*
    * Function wich inits exception handler to specific callback 
    */
    inline void init() {
        SetUnhandledExceptionFilter(CrashExceptionFilter);
    }
};
