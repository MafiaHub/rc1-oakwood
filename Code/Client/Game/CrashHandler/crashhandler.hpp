#pragma once
namespace crashhandler
{
    /*
    * Method for posting crash report to the server
    * that creates new process for sending data 
    */
    inline void send_report(char* report_buffer) {

        STARTUPINFOA siStartupInfo;
        PROCESS_INFORMATION piProcessInfo;
        memset(&siStartupInfo, 0, sizeof(siStartupInfo));
        memset(&piProcessInfo, 0, sizeof(piProcessInfo));
        siStartupInfo.cb = sizeof(siStartupInfo);

        char starting_param[1000];
        sprintf(starting_param, "bin\\oakreport.exe \"%s|%s|%s\"", GlobalConfig.username.c_str(), 
            GlobalConfig.server_address.c_str(), report_buffer);
        
        if (!CreateProcessA(NULL,
            starting_param, 0, 0, false,
            CREATE_SUSPENDED, 0, 0,
            &siStartupInfo, &piProcessInfo)) {
            MessageBoxA(NULL, "Creating proccess failed !", "Error", MB_OK);
        }
        DWORD pId = piProcessInfo.dwProcessId;
        ResumeThread(piProcessInfo.hThread);
    }

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
        
        char buff[500];
        char address[100];
        GetAdressAsModule((DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress, address);
        sprintf(buff, "Crash occured at address: %p Code: %x \nRegisters: \nEAX: %p \tECX: %p\nEDX: %p \tEBX: %p\nESP: %p \tEBP: %p\nESI: %p \tEDI: %p\nModule: %s\n",
            ExceptionInfo->ExceptionRecord->ExceptionAddress,
            ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ContextRecord->Eax,
            ExceptionInfo->ContextRecord->Ecx, ExceptionInfo->ContextRecord->Edx, ExceptionInfo->ContextRecord->Ebx,
            ExceptionInfo->ContextRecord->Esp, ExceptionInfo->ContextRecord->Ebp, ExceptionInfo->ContextRecord->Esi,
            ExceptionInfo->ContextRecord->Edi, address);
        
        send_report(buff);
        FILE* log = fopen("crashdump.txt", "a");
        if (log) {
            fprintf(log, "---\nMINIDUMP\n%s\n", buff);
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
