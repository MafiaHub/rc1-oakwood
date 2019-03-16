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

#define SCREEN_BUFFER_SIZE 256

namespace console {
    struct _console_data {
        char info_tag[80];
        int loader_state;
    #ifdef _WIN32
        CHAR_INFO screen_buffer[SCREEN_BUFFER_SIZE];
    #else
        //WINDOW* wnd_menu = nullptr;
    #endif
    } console_data;

    inline void init();
    inline void draw(const char* format, ...);
    inline void printf(const char* format, ...);
    inline char loader();

    #ifdef _WIN32 
    inline void init_w32() {
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

    inline void draw_w32(const char* format, va_list va) {
        auto std_out = GetStdHandle(STD_OUTPUT_HANDLE);
        char formated_tag[180];
        vsprintf(formated_tag, format, va);

        CONSOLE_SCREEN_BUFFER_INFO buffer_info;
        if (!GetConsoleScreenBufferInfo(std_out, &buffer_info))
            return;

        buffer_info.dwSize.X = zpl_min(buffer_info.dwSize.X, SCREEN_BUFFER_SIZE);
        if (console_data.screen_buffer == NULL || strcmp(formated_tag, console_data.info_tag)) {
            int buffer_count = 0;
            strcpy(console_data.info_tag, formated_tag);

            for (int i = 0; i < buffer_info.dwSize.X; i++) {
                if (formated_tag[i] == NULL) {
                    for (int j = buffer_count; j < buffer_info.dwSize.X; j++) {
                        console_data.screen_buffer[j].Char.UnicodeChar = L' ';
                        console_data.screen_buffer[j].Attributes = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
                    }
                    break;
                }
                else {
                    switch ((unsigned char)(formated_tag[i])) {
                    case 128:
                        console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                        break;
                    case 129:
                        console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                        break;
                    case 130:
                        console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_RED;
                        break;
                    case 131:
                        console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_GREEN;
                        break;
                    case 132:
                        console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_BLUE;
                        break;
                    case 133:
                        console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED;
                        break;
                    case 134:
                        console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
                        break;
                    case 135:
                        console_data.screen_buffer[buffer_count].Attributes = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
                        break;
                    default:
                        console_data.screen_buffer[buffer_count].Attributes = 0;
                        break;
                    }

                    if ((unsigned char)formated_tag[i] > 127)
                        i++;

                    console_data.screen_buffer[buffer_count].Char.UnicodeChar = formated_tag[i];
                    console_data.screen_buffer[buffer_count++].Attributes |= (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
                }
            }
        }

        COORD      BufferSize = { buffer_info.dwSize.X, 1 };
        COORD      TopLeft = { 0, buffer_info.srWindow.Top };
        SMALL_RECT Region = { 0, buffer_info.srWindow.Top, buffer_info.dwSize.X, 1 };
        WriteConsoleOutputW(std_out, console_data.screen_buffer, BufferSize, TopLeft, &Region);
    }

    #else
    inline void init_unix() {

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

        console_data.wnd_menu = subwin(stdscr, 1, COLS, 0, 0);
        wbkgd(console_data.wnd_menu, COLOR_PAIR(1));
        move(1, 0);
        refresh(); */
    }

    inline void draw_unix(const char* format, va_list va) {
        vprintf(format, va);
        /* int attr = COLOR_PAIR(1);
        wmove(console_data.wnd_menu, 0, 0);

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
                waddch(console_data.wnd_menu, formated_tag[i] | attr);
                attr = COLOR_PAIR(1);
                break;
            }
        }

        if (console_data.wnd_menu) {
            wnoutrefresh(console_data.wnd_menu);
            doupdate();
            refresh();
        } */
    }
    #endif

    inline void init() {
    #ifdef _WIN32
        init_w32();
    #else
        init_unix();
    #endif
    }

    inline void draw(const char* format, ...) {
        va_list arglist;
        va_start(arglist, format);
    #ifdef _WIN32
        draw_w32(format, arglist);
    #else
        draw_unix(format, arglist);
    #endif
        va_end(arglist);
    }

    inline void printf(const char* format, ...) {
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

    char update_loader() {
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
}