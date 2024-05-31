#pragma once

#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <string>
#include "XOsRequestFormat.h"


#define DEFAULT_BUFFER_LENGTH 512
#define DEFAULTPORT "5208"
#define CLIENT_IDLE 0
#define CLIENT_WAITING 1
#define CLIENT_INGAME 2
#define NO_MOVE_FLAG 0
#define INVALID_MOVE 1
#define GAME_WON 2

#pragma comment(lib, "Ws2_32.lib")
class XOsClient {
public:
	XOsClient();
	~XOsClient();
	void clientActive();
	void clientJoin();
private:
	void createClient();
	void clientError(const std::string&);
	void deserializeData(char* recvBuffer);
	void displayConnection(addrinfo*);
	void seralizeAndSendData(XOsRequestType rt, char* payload, char payloadSize);
	void outputRequest(char *);
	void closeClient();
	char formatMove(char);
	int m_socket{};
	int m_debug{ true };
	char m_id{-1};
	char m_currentGame{ -1 };
	int m_state{ CLIENT_IDLE };
	std::string m_userName;
};