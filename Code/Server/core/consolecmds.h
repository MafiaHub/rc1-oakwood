#pragma once

using namespace std;

typedef function<void(vector<string>)> CmdCallback;

map<string, CmdCallback> cmds;

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
    addCmd("reload", [](vector<string> args) {
        oak_server_reload();
    });
    addCmd("ver", [](vector<string> args) {
        oak_console_printf("^FMafia: ^9Oakwood ^Av%s ^8(^B%s Build^8)^R\n", OAK_VERSION, OAK_BUILD_TYPE);
        oak_console_printf("^FBuild date: ^A%s^R\n", OAK_BUILD_DATE);
        oak_console_printf("^FBuild time: ^A%s^R\n", OAK_BUILD_TIME);
    });
    addCmd("quit", [](vector<string> args) {
        shutdown_server();
    });
    addCmd("q", [](vector<string> args) {
        shutdown_server();
    });
    addCmd("list", [](vector<string> args) {
        auto players = oak_player_list(0);
        if (GlobalConfig.players == 0)
        {
            oak_console_printf("^FThere are no players connected.^R\n");
            return;
        }

        int playersInRow = 5;
        int actualPlayersInRow = 0;

        oak_console_printf("^FConnected players ^8(^A%d^8)^F:^R\n\t", GlobalConfig.players);
        for (int i = 0; i < GlobalConfig.players; i++)
        {
            if (actualPlayersInRow < playersInRow && i + 1 < GlobalConfig.players)
            {
                oak_console_printf("^A%s^F, ", oak_player_name_get(players[i]));
                actualPlayersInRow++;
            }
            else
            {
                oak_console_printf("^A%s^R\n", oak_player_name_get(players[i]));
                actualPlayersInRow = 0;
            }
        }
    });
    addCmd("kick", [](vector<string> args) {
        if (args.size() < 2)
        {
            oak_console_printf("^F[^5USAGE^F] ^Akick ^B<playerID> <reason>^R\n");
            return;
        }

        std::string reason;

        for (int x = 1; x < args.size(); x++)
        {
            if (x == args.size() - 1)
            {
                reason += args[x];
            }
            else
            {
                reason += args[x] + " ";
            }
        }

        auto players = oak_player_list(0);
        for (int i = 0; i < GlobalConfig.players; i++)
        {
            if (players[i] == atoi(args[0].c_str()))
            {
                const char* name = oak_player_name_get(players[i]);
                oak_player_kick(players[i], reason.c_str(), reason.size());
                oak_console_printf("^F[^5INFO^F] Player ^A%s ^Fwas kicked. Reason: ^A%s^R\n", name, reason.c_str());
                return;
            }
        }
        oak_console_printf("^F[^5ERROR^F] Player not found.^R\n");
    });
}

bool execCmd(string cmdName, vector<string> args)
{
    if (!cmdExists(cmdName)) return false;
    
    cmds[cmdName](args);
    return true;
}
