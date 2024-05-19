#pragma once

#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <string>
#include "XOsRequestFormat.h"


#define DEFAULT_BUFFER_LENGTH 512
#define DEFAULTPORT "5208"

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
	int m_socket{};
	int m_debug{ true };
	int m_id{};
	std::string m_userName;
};