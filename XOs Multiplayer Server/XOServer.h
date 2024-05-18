#pragma once

#include <iostream>
#include <winsock2.h>
//#include <winsock.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <string>
#include "XOsRequestResponse.h"
#define DEFAULT_BUFFER_LENGTH 512
#define DEFAULTPORT "5208"
#define HEADER_SIZE 2


#pragma comment(lib, "Ws2_32.lib")
class XOsServer {
public:
	XOsServer();
	~XOsServer();
	void acceptConnections();
	void beginListen();
	void deserializeData(char* recvBuffer);
private:
	void createServer();
	void serverError(const std::string &);
	void displayConnection(addrinfo *);
	int m_socket{};
	int m_debug{ true };
};