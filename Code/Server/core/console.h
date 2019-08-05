#pragma once
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#else
#include <unistd.h>
#include <err.h>
#include <string.h>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <atomic>

#define OAK_CONSOLE_SCREEN_BUFFER_SIZE 256

struct {
    char info_tag[80];
    int loader_state;
    zpl_thread input_handler;
    zpl_atomic32 input_block;
    zpl_atomic32 input_handler_running;
#ifdef _WIN32
    CHAR_INFO screen_buffer[OAK_CONSOLE_SCREEN_BUFFER_SIZE];
#else
    //WINDOW* wnd_menu = nullptr;
#endif
} oak__console_data;

void oak_console_init();
void oak_console_draw(const char* format, ...);
void oak_console_printf(const char* format, ...);

zpl_isize oak__console_input_handler(struct zpl_thread *t) {
    do {
        std::string line;
        std::getline(std::cin, line);

        while (zpl_atomic32_load(&oak__console_data.input_block) > 0) {
            zpl_yield_thread();
        }

        oak_bridge_event_console(line.c_str());
    } while(zpl_atomic32_load(&oak__console_data.input_handler_running) > 0);

    return 0;
}

void oak_console_input_handler_init() {
    zpl_atomic32_store(&oak__console_data.input_handler_running, 1);
    zpl_thread_init(&oak__console_data.input_handler);
    zpl_thread_start(&oak__console_data.input_handler, oak__console_input_handler, NULL);
}

void oak_console_input_handler_destroy() {
    zpl_atomic32_store(&oak__console_data.input_handler_running, 0);
    zpl_atomic32_store(&oak__console_data.input_block, 0);
    //zpl_thread_destroy(&oak__console_data.input_handler);
}

void oak_console_block_input(int state) {
    zpl_atomic32_store(&oak__console_data.input_block, state);
}

#ifdef _WIN32
inline void oak__console_init_w32() {
    auto std_in = GetStdHandle(STD_INPUT_HANDLE);
    auto std_out = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD dwConInMode;
    GetConsoleMode(std_in, &dwConInMode);
    SetConsoleMode(std_in, dwConInMode & ~ENABLE_QUICK_EDIT_MODE);
    SetConsoleTextAttribute(std_out, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    if (!GetConsoleScreenBufferInfo(std_out, &buffer_info))
        return;

    buffer_info.dwSize.Y = buffer_info.srWindow.Bottom + 1;
    SetConsoleWindowInfo(std_out, TRUE, &buffer_info.srWindow);
    SetConsoleScreenBufferSize(std_out, buffer_info.dwSize);
    SetConsoleOutputCP(CP_UTF8);
}

inline void oak__console_draw_w32(const char* format, va_list va) {
    auto std_out = GetStdHandle(STD_OUTPUT_HANDLE);
    char formated_tag[180];
    vsprintf(formated_tag, format, va);

    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    if (!GetConsoleScreenBufferInfo(std_out, &buffer_info))
        return;

    buffer_info.dwSize.X = zpl_min(buffer_info.dwSize.X, OAK_CONSOLE_SCREEN_BUFFER_SIZE);
    if (oak__console_data.screen_buffer == NULL || strcmp(formated_tag, oak__console_data.info_tag)) {
        int buffer_count = 0;
        strcpy(oak__console_data.info_tag, formated_tag);

        for (int i = 0; i < buffer_info.dwSize.X; i++) {
            if (formated_tag[i] == NULL) {
                for (int j = buffer_count; j < buffer_info.dwSize.X; j++) {
                    oak__console_data.screen_buffer[j].Char.UnicodeChar = L' ';
                    oak__console_data.screen_buffer[j].Attributes = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
                }
                break;
            }
            else {
                switch ((unsigned char)(formated_tag[i])) {
                case 128:
                    oak__console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                    break;
                case 129:
                    oak__console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                    break;
                case 130:
                    oak__console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_RED;
                    break;
                case 131:
                    oak__console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_GREEN;
                    break;
                case 132:
                    oak__console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_BLUE;
                    break;
                case 133:
                    oak__console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED;
                    break;
                case 134:
                    oak__console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
                    break;
                case 135:
                    oak__console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
                    break;
                default:
                    oak__console_data.screen_buffer[buffer_count].Attributes = 0;
                    break;
                }

                if ((unsigned char)formated_tag[i] > 127)
                    i++;

                oak__console_data.screen_buffer[buffer_count].Char.UnicodeChar = formated_tag[i];
                oak__console_data.screen_buffer[buffer_count++].Attributes |= (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
            }
        }
    }

    COORD      BufferSize = { buffer_info.dwSize.X, 1 };
    COORD      TopLeft = { 0, buffer_info.srWindow.Top };
    SMALL_RECT Region = { 0, buffer_info.srWindow.Top, buffer_info.dwSize.X, 1 };
    WriteConsoleOutputW(std_out, oak__console_data.screen_buffer, BufferSize, TopLeft, &Region);
}

#else
inline void oak__console_init_unix() {

    /* std::setlocale(LC_ALL, "");
    std::setlocale(LC_NUMERIC, "C");
    std::setlocale(LC_COLLATE, "C");

    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    idlok(stdscr, FALSE);
    scrollok(stdscr, TRUE);
    setscrreg(1, LINES - 1);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_BLACK, COLOR_GREEN);
        init_pair(3, COLOR_WHITE, COLOR_WHITE);
        init_pair(4, COLOR_RED, COLOR_WHITE);
        init_pair(5, COLOR_GREEN, COLOR_WHITE);
        init_pair(6, COLOR_BLUE, COLOR_WHITE);
    }

    oak__console_data.wnd_menu = subwin(stdscr, 1, COLS, 0, 0);
    wbkgd(oak__console_data.wnd_menu, COLOR_PAIR(1));
    move(1, 0);
    refresh(); */
}

inline void oak__console_draw_unix(const char* format, va_list va) {
    vprintf(format, va);
    /* int attr = COLOR_PAIR(1);
    wmove(oak__console_data.wnd_menu, 0, 0);

    char formated_tag[200];
    vsprintf(formated_tag, format, va);

    for (int i = 0; i < COLS; i++) {

        if (formated_tag[i] == '\0')
            break;

        switch ((unsigned char)(formated_tag[i])) {
        case 128:
            attr = COLOR_PAIR(3) | A_BOLD;
            break;
        case 129:
            attr = COLOR_PAIR(3);
            break;
        case 130:
            attr = COLOR_PAIR(4);
            break;
        case 131:
            attr = COLOR_PAIR(5);
            break;
        case 132:
            attr = COLOR_PAIR(6);
            break;
        case 133:
            attr = COLOR_PAIR(4) | A_BOLD;
            break;
        case 134:
            attr = COLOR_PAIR(5) | A_BOLD;
            break;
        case 135:
            attr = COLOR_PAIR(6) | A_BOLD;
            break;
        default:
            waddch(oak__console_data.wnd_menu, formated_tag[i] | attr);
            attr = COLOR_PAIR(1);
            break;
        }
    }

    if (oak__console_data.wnd_menu) {
        wnoutrefresh(oak__console_data.wnd_menu);
        doupdate();
        refresh();
    } */
}
#endif

inline void oak_console_init() {
#ifdef _WIN32
    oak__console_init_w32();
#else
    oak__console_init_unix();
#endif
}

inline void oak_console_draw(const char* format, ...) {
    va_list arglist;
    va_start(arglist, format);
#ifdef _WIN32
    oak__console_draw_w32(format, arglist);
#else
    oak__console_draw_unix(format, arglist);
#endif
    va_end(arglist);
}

inline void oak_console_printf(const char* format, ...) {
    va_list arglist;
    va_start(arglist, format);
#ifdef _WIN32
    vprintf(format, arglist);
#else
    //vwprintw(stdscr, format, arglist);
    vprintf(format, arglist);
#endif
    va_end(arglist);
}

char oak_console_update_loader() {
    static char cnt = -1;
    cnt++;
    if (cnt > 3)
        cnt = 0;

    switch (cnt) {
    case 0:
        return '/';
    case 1:
        return '-';
    case 2:
        return '\\';
    case 3:
        return '|';
    }
    return ' ';
}

zpl_global f64 oak__console_last_console_update  = 0.0f;
zpl_global f64 oak__console_last_fps_update      = 0.0f;
zpl_global u32 oak__console_fps_counter          = 0;
zpl_global u32 oak__console_computed_fps         = 0;

void oak_console_console_update_stats() {
    #ifndef _WIN32
        return;
    #endif

    f64 current_time = zpl_time_now();
    f64 diff = current_time - oak__console_last_fps_update;

    oak__console_fps_counter++;
    if (diff >= 1.0) {
        oak__console_computed_fps = oak__console_fps_counter;
        oak__console_fps_counter = 0;
        oak__console_last_fps_update = current_time;
    }

    //NOTE(DavoSK): Update our debug tag every 200ms
    if (current_time - oak__console_last_console_update > 0.2f) {
        oak_console_draw("%c[%c%c%c] Oakwood Server | NET: %dKB / %dKB | TPS: %d (%.02f ms) | Players: %d / %d",
            132,
            130,
            oak_console_update_loader(),
            132,
            oak_network_ctx_get()->network.host->totalReceivedData / 1024,
            oak_network_ctx_get()->network.host->totalSentData / 1024,
            oak__console_computed_fps,
            1000.0f / oak__console_computed_fps,
            (u32)GlobalConfig.players,
            (u32)GlobalConfig.max_players);

        oak__console_last_console_update = current_time;
    }
}
