// XOs Multiplayer Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "XOServer.h"
int main()
{
    XOsServer server = XOsServer();
    server.beginListen();
    server.acceptConnections();
}
