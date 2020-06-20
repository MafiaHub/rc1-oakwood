#pragma once

enum
{
    DIALOG_MSGBOX,
    DIALOG_INPUT,
    DIALOG_PASSWORD,
};

struct dialog_data
{
    int dialogId, dialogType;
    const char *title, 
               *message, 
               *button1, 
               *button2;
};
