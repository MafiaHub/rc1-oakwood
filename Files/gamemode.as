void onPlayerConnect(int playerId)
{
    print("Player " + playerGetName(playerId) + " joined the game.\n");
    playerSpawn(playerId, -2132.5144f, -5.7746344f, -521.1489f, 90.0f);
}

void onPlayerDisconnect(int playerId)
{
    print("Player " + playerGetName(playerId) + " left the game.\n");
}

void onPlayerChat(int playerId, string text)
{
    print("<" + playerGetName(playerId) + ">: " + text + "\n");
    allSendMessage("<" + playerGetName(playerId) + ">: " + text);
}

void onConsoleInput(string text)
{
    if(text == "") return;
    print("Input: '" + text + "'\n");
    allSendMessage("<Console>: " + text);
}

void main()
{
    print("^AHello from this script. ^E:)^R\n");
    print("^FServer name: ^A" + serverName + "^R\n");
    print("^FMax players: ^A" + maxPlayers + "^R\n");
}