#pragma once

#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <string>
#define DEFAULT_BUFFER_LENGTH 512
#define DEFAULTPORT "5208"

#pragma comment(lib, "Ws2_32.lib")
class XOsClient {
public:
	XOsClient();
	~XOsClient();
	void serverError();
	void openConnection();
	void sendData();
private:
	void createClient();
	void serverError(const std::string&);
	void displayConnection(addrinfo*);
	int m_socket{};
};