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
	void serverError();
	void openConnection();
	void clientActive();
private:
	void createClient();
	void serverError(const std::string&);
	void displayConnection(addrinfo*);
	void seralizeAndSendData(XOsRequestType rt, char* payload, char payloadSize);
	int m_socket{};
};