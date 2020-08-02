#pragma once
#if defined(ZPL_SYSTEM_WINDOWS) && !defined(_DEBUG) && !defined(__GNUC__)
#include <DbgHelp.h>
#endif

#if defined(ZPL_SYSTEM_WINDOWS)
#include <shellapi.h>
#include <winternl.h>
#include <ntstatus.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#endif

#if defined(ZPL_SYSTEM_WINDOWS) && !defined(_DEBUG) && !defined(__GNUC__)
bool GetTrueWindowsVersion(OSVERSIONINFOEX* pOSversion)
{
    // Function pointer to driver function
    NTSTATUS(WINAPI * pRtlGetVersion)(
        PRTL_OSVERSIONINFOW lpVersionInformation) = NULL;

    // Loading the ntdll - NT Kernel controller
    HINSTANCE hNTdllDll = LoadLibrary("ntdll.dll");

    // Is it loaded?
    if (hNTdllDll != NULL)
    {
        // Get the function pointer to RtlGetVersion
        pRtlGetVersion = (NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW))
            GetProcAddress(hNTdllDll, "RtlGetVersion");

        // If successfull then read the function
        if (pRtlGetVersion != NULL)
            pRtlGetVersion((PRTL_OSVERSIONINFOW)pOSversion);

        // Free the library
        FreeLibrary(hNTdllDll);
    } // if (hNTdllDll != NULL)

    // If function failed, use fallback to old version
    if (pRtlGetVersion == NULL)
        GetVersionEx((OSVERSIONINFO*)pOSversion);

    // Always true ...
    return true;
}

bool isServer()
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0, 0, VER_NT_WORKSTATION };
    DWORDLONG        const dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);

    return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
}

char* getWinVerName(int major, int minor)
{
    if (major == 10 && minor == 0) return isServer() ? "Windows Server 2016 or 2019" : "Windows 10";
    else if (major == 6 && minor == 3) return isServer() ? "Windows Server 2012 R2" : "Windows 8.1";
    else if (major == 6 && minor == 2) return isServer() ? "Windows Server 2012" : "Windows 8";
    else if (major == 6 && minor == 1) return isServer() ? "Windows Server 2008 R2" : "Windows 7";
    else if (major == 6 && minor == 0) return isServer() ? "Windows Server 2008" : "Windows Vista";
    else if (major == 5 && minor == 2) return isServer() ? "Windows Server 2003 (R2)" : "Windows XP x64 Edition";
    else if (major == 5 && minor == 1) return "Windows XP";
}

void oak_crash_handler(unsigned int type, EXCEPTION_POINTERS* ep)
{
    EXCEPTION_RECORD* er = ep->ExceptionRecord;
    CONTEXT* context = ep->ContextRecord;
    MINIDUMP_EXCEPTION_INFORMATION ExpInfo;

    char out[4096];
    char msg[512];

    std::string buf = "";

    SymInitialize(GetCurrentProcess(), NULL, TRUE);

    STACKFRAME sf = { {context->Eip, 0, AddrModeFlat}, {}, {context->Ebp, 0, AddrModeFlat}, {context->Esp, 0, AddrModeFlat}, 0 };
    while (::StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &sf, context, NULL, ::SymFunctionTableAccess, ::SymGetModuleBase, NULL))
    {
        union { IMAGEHLP_SYMBOL sym; char symext[sizeof(IMAGEHLP_SYMBOL) + sizeof(std::string)]; };
        sym.SizeOfStruct = sizeof(sym);
        sym.MaxNameLength = sizeof(symext) - sizeof(sym);
        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(line);
        DWORD symoff, lineoff;
        if (SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &symoff, &sym) && SymGetLineFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &lineoff, &line))
        {
            char* del = strrchr(line.FileName, '\\');
            char dline[128];
            sprintf(dline, "0x%X %s > %s - line %d\n", sym.Address, sym.Name, line.FileName, line.LineNumber);
            buf += std::string(dline);
        }
    }

    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    char buffer2[160];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H-%M-%S", timeinfo);
    strftime(buffer2, sizeof(buffer2), "%d.%m.%Y - %H:%M:%S", timeinfo);
    char* time(buffer);
    char* time2(buffer2);

    char registers[1024];

    sprintf(registers, "Segment registers:\n\tSegDs: 0x%08X\n\tSegEs: 0x%08X\n\tSegFs: 0x%08X\n\tSegGs: 0x%08X\nDebug registers:\n\tDr0: 0x%08X\n\tDr1: 0x%08X\n\tDr2: 0x%08X\n\tDr3: 0x%08X\n\tDr6: 0x%08X\n\tDr7: 0x%08X\nInteger registers:\n\tEDI: 0x%08X\n\tESI: 0x%08X\n\tEBX: 0x%08X\n\tEDX: 0x%08X\n\tECX: 0x%08X\n\tEAX: 0x%08X\nControl registers:\n\tEBP: 0x%08X\n\tEIP: 0x%08X\n\tSegCs: 0x%08X\n\tEFlags: 0x%08X\n\tESP: 0x%08X\n\tSegSs: 0x%08X", context->SegDs, context->SegEs, context->SegFs, context->SegGs, context->Dr0, context->Dr1, context->Dr2, context->Dr3, context->Dr6, context->Dr7, context->Edi, context->Esi, context->Ebx, context->Edx, context->Ecx, context->Eax, context->Ebp, context->Eip, context->SegCs, context->EFlags, context->Esp, context->SegSs);

    char dir[512];
    char name[640];
    char content[10240];

    const char* expDesc;

    switch (er->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        expDesc = "Access Violation";
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        expDesc = "Array bounds Exceeded";
        break;
    case EXCEPTION_BREAKPOINT:
        expDesc = "Break point";
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        expDesc = "DataType Misalignment";
        break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        expDesc = "Float - Denormal Float Operand";
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        expDesc = "Float - Number divided by zero";
        break;
    case EXCEPTION_FLT_INEXACT_RESULT:
        expDesc = "Float - Inexact Result";
        break;
    case EXCEPTION_FLT_OVERFLOW:
        expDesc = "Float - Overflow";
        break;
    case EXCEPTION_FLT_STACK_CHECK:
        expDesc = "Float - Stack Overflow/Underflow";
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        expDesc = "Float - Underflow";
        break;
    case EXCEPTION_GUARD_PAGE:
        expDesc = "Memory allocated with PAGE_GUARD";
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        expDesc = "Illegal CPU instruction";
        break;
    case EXCEPTION_IN_PAGE_ERROR:
        expDesc = "System is unable to load the page";
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        expDesc = "Integer - Number divided by zero";
        break;
    case EXCEPTION_INT_OVERFLOW:
        expDesc = "Integer - Overflow";
        break;
    case EXCEPTION_INVALID_DISPOSITION:
        expDesc = "Invalid Disposition";
        break;
    case EXCEPTION_INVALID_HANDLE:
        expDesc = "Invalid Handle";
        break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        expDesc = "Non-continuable Exception";
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        expDesc = "Instruction not allowed on this PC";
        break;
    case EXCEPTION_SINGLE_STEP:
        expDesc = "Trace Trap";
        break;
    case EXCEPTION_STACK_OVERFLOW:
        expDesc = "Stack Overflow";
        break;
    default:
        expDesc = "Unknown Exception";
        break;
    }

    OSVERSIONINFOEX osVer;
    ZeroMemory(&osVer, sizeof(OSVERSIONINFOEX));
    osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetTrueWindowsVersion(&osVer);

    sprintf(content, "Oakwood Server crash report created at %s.\nUnhandled exception at 0x%08X (0x%08X - %s)\n\n=============[VERSION INFO]=============\nVersion: %s\nBuild date: %s\nBuild time: %s\n========================================\n\n=============[SYSTEM INFO]=============\nOS: %s (%d.%d.%d)\nIs Windows Server: %s\n=======================================\n\n============[CPU REGISTERS]============\n%s\n=======================================\n\n==============[CALL STACK]==============\n%s========================================", time2, er->ExceptionAddress, er->ExceptionCode, expDesc, OAK_VERSION, __DATE__, __TIME__, getWinVerName(osVer.dwMajorVersion, osVer.dwMinorVersion), osVer.dwMajorVersion, osVer.dwMinorVersion, osVer.dwBuildNumber, isServer() ? "Yes" : "No", registers, buf.c_str());

    sprintf(dir, "crash-reports");

    CreateDirectory(dir, NULL);

    sprintf(name, "%s\\server_%s.txt", dir, time);

    std::ofstream file;
    file.open(name);
    file << content;
    file.close();

    oak_console_input_handler_destroy();

    char errorText[512];
    sprintf(errorText, "^F[^9ERROR^F] Unhandled exception ^A0x%08X ^8(^B%s^8)^F at ^A0x%08X^R\n\t^AMore information available in ^B%s^R\n", er->ExceptionCode, expDesc, er->ExceptionAddress, name);

    oak_console_printf(errorText);

    if (!GlobalConfig.crash_restart)
    {
        oak_console_printf("Press any key to continue...\n");
        _getch();
        zpl_exit(1);
    }
    else
    {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        CreateProcessA
        (
            __argv[0],
            NULL,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi
        );

        // Close process and thread handles. 
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        zpl_exit(1);
    }
}
#endif
