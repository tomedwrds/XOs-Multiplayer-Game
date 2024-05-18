#pragma once

#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <string>
#include "XOsRequestFormat.h"
#include <thread>
#include <vector>
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
	int m_socket{};
	int m_debug{ true };
};