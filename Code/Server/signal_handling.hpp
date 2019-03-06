void shutdown_server();
void mod_log(const char* msg);

#include <signal.h>
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    #ifdef ZPL_SYSTEM_WINDOWS

    BOOL WINAPI win32_control_handler(DWORD control_type)
    {
        switch (control_type)
        {
        case CTRL_C_EVENT:
        case DBG_CONTROL_C:
            mod_log("Ctrl-C pressed, stopping the server...");
            shutdown_server();
            return 0;
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            shutdown_server();
            return 1;
        }

        return 0;
    }
    #else //POSIX complaint
    void posix_signal_handler(int sig) {
        shutdown_server();
    }
    #endif
#endif

void register_console_events() {
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    mod_log("Installing signal handlers...");
    #ifdef ZPL_SYSTEM_WINDOWS
    {
        if (!SetConsoleCtrlHandler(win32_control_handler, 1)) {
            mod_log("Could not set up signal handler!");
        }
    }
    #else // POSIX compliant
    signal(SIGINT, &posix_signal_handler);
    signal(SIGTERM, &posix_signal_handler);
    #endif
#endif
}

void unregister_console_events() {
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    mod_log("Installing signal handlers...");
    #ifdef ZPL_SYSTEM_WINDOWS
    {
        if (!SetConsoleCtrlHandler(win32_control_handler, 0)) {
            mod_log("Could not uninstall signal handler!");
        }
    }
    #else // POSIX compliant
    #endif
#endif
}
