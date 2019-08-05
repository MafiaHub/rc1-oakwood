void shutdown_server();

#include <signal.h>
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    #ifdef ZPL_SYSTEM_WINDOWS

    BOOL WINAPI oak__sighandler_win32_control_handler(DWORD control_type)
    {
        switch (control_type)
        {
        case CTRL_C_EVENT:
        case DBG_CONTROL_C:
            oak_log("Ctrl-C pressed, stopping the server...\n");
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
    void oak__sighandler_posix_signal_handler(int sig) {
        shutdown_server();
    }
    #endif
#endif

void oak_sighandler_register() {
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    oak_log("Installing signal handlers...\n");
    #ifdef ZPL_SYSTEM_WINDOWS
    {
        if (!SetConsoleCtrlHandler(oak__sighandler_win32_control_handler, 1)) {
            oak_log("Could not set up signal handler!\n");
        }
    }
    #else // POSIX compliant
    signal(SIGINT, &oak__sighandler_posix_signal_handler);
    signal(SIGTERM, &oak__sighandler_posix_signal_handler);
    #endif
#endif
}

void oak_sighandler_unregister() {
#ifndef OAK_DISABLE_SIGNAL_HANDLING
    oak_log("Installing signal handlers...\n");
    #ifdef ZPL_SYSTEM_WINDOWS
    {
        if (!SetConsoleCtrlHandler(oak__sighandler_win32_control_handler, 0)) {
            oak_log("Could not uninstall signal handler!");
        }
    }
    #else // POSIX compliant
    #endif
#endif
}
