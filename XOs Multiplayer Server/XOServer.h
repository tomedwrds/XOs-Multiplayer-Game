#pragma once

#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <string>
#include "XOsRequestFormat.h"
#include <thread>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include "Game.h"
#define DEFAULT_BUFFER_LENGTH 512
#define DEFAULTPORT "5208"


#pragma comment(lib, "Ws2_32.lib")
class XOsServer {
public:
	XOsServer();
	~XOsServer();
	void acceptConnection();
	void beginListen();
	void serverActive(int);
private:
	void createServer();
	void serverError(const std::string &);
	void displayConnection(addrinfo *);
	void deserializeData(char* recvBuffer, int);
	void seralizeAndSendData(XOsRequestType rt, char* payload, char payloadSize, int);
	void outputRequest(char*);
	int m_socket{};
	int m_debug{ true };
	std::map<char, Game> m_games;
	char m_gameId{ 0 };
	char m_totalUsers{ 0 };
	std::map<std::string, char> m_users;
	std::map<char, std::map<char, bool>> m_challenges;
};