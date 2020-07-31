#pragma once

using namespace std;

typedef function<void(vector<string>)> CmdCallback;

map<string, CmdCallback> cmds;

void reloadServer(vector<string> args)
{
    oak_server_reload();
}

void getServerVer(vector<string> args)
{
    oak_console_printf("^FMafia: ^9Oakwood ^Av%s ^8(^B%s Build^8)^R\n", OAK_VERSION, OAK_BUILD_TYPE);
    oak_console_printf("^FBuild date: ^A%s^R\n", OAK_BUILD_DATE);
    oak_console_printf("^FBuild time: ^A%s^R\n", OAK_BUILD_TIME);
}

void quitServer(vector<string> args)
{
	shutdown_server();
}

bool cmdExists(string cmdName)
{
    map<string, CmdCallback>::iterator it = cmds.find(cmdName);
    if (it != cmds.end())
    {
        return true;
    }

    return false;
}

bool addCmd(string cmdName, CmdCallback callback)
{
    if (cmdExists(cmdName)) return false;

    cmds[cmdName] = callback;
    return true;
}

void registerCmds()
{
    addCmd("reload", reloadServer);
    addCmd("ver", getServerVer);
    addCmd("quit", quitServer);
    addCmd("q", quitServer);
}

bool execCmd(string cmdName, vector<string> args)
{
    if (!cmdExists(cmdName)) return false;
    
    cmds[cmdName](args);
    return true;
}
