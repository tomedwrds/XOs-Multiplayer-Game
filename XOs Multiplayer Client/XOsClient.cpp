#include "XOsClient.h"

XOsClient::XOsClient() {
    createClient();
}

XOsClient::~XOsClient() {
    closesocket(m_socket);
    WSACleanup();
    exit(0);
}


void XOsClient::createClient() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        serverError("Failed WSA Startup");
    }

    struct addrinfo* result, hints, *ptr;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    if (getaddrinfo("127.0.0.1", DEFAULTPORT, &hints, &result) < 0) {
        serverError("Failed to listen to get address info.");
    }

    ptr = result;
    m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (m_socket == INVALID_SOCKET) {
        serverError("Failed to listen to socket.");
    }

    if (connect(m_socket, result->ai_addr, (int)result->ai_addrlen) < 0) {
        serverError("Failed to bind to connect.");
    }
    displayConnection(result);
    freeaddrinfo(result);
}

void XOsClient::seralizeAndSendData(XOsRequestType rt, char* payload, char payloadSize) {
    char* sendBuffer = new char[payloadSize + HEADER_SIZE];
    
    sendBuffer[0] = rt;
    sendBuffer[1] = payloadSize;
    
    for (int i = HEADER_SIZE; i < payloadSize + HEADER_SIZE; i++) {
        sendBuffer[i] = (char)payload[i - HEADER_SIZE];
    }
    if (send(m_socket, sendBuffer, payloadSize + HEADER_SIZE, 0) < 0) {
        serverError("Failed to send data");
    }
    delete[] sendBuffer;
}



void XOsClient::clientActive() {

    char data[100]{ "hello world" };
    seralizeAndSendData(XOsRequestType::JOIN, ((char*)data), (char) strlen(data) + 1);
    

 
    if (shutdown(m_socket, SD_SEND) < 0) {
        serverError("Failed to shutdown");
    }

    while (1);
}



void XOsClient::serverError(const std::string& errorMessage) {
    std::cout << "ERROR: " << errorMessage << " CODE: " << WSAGetLastError() << '\n';
    closesocket(m_socket);
    WSACleanup();
    exit(1);
}

void XOsClient::displayConnection(addrinfo* addressInfo) {
    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    if (getnameinfo(addressInfo->ai_addr, addressInfo->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
        serverError("Failed getting connection information.");
    }
    std::cout << "Server opened conenction on ip: " << ip << ":" << port << '\n';
}