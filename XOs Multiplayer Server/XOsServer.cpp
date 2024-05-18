#include "XOServer.h"

XOsServer::XOsServer() {
	createServer();
}

XOsServer::~XOsServer() {
    closesocket(m_socket);
    WSACleanup();
    exit(0);
}

void XOsServer::createServer() {
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        serverError("Failed WSA Startup");
    }
    
    //Create server
    struct addrinfo* result, hints;
    memset(&hints, 0, sizeof(hints));
    
    //Assign hints
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

  
    if (getaddrinfo(NULL, DEFAULTPORT, &hints, &result) < 0) {
        serverError("Failed to listen to get address info.");
    }

    //Create socket
    m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_socket == INVALID_SOCKET) {
        serverError("Failed to listen to socket.");
    }

    if (bind(m_socket, result->ai_addr, (int)result->ai_addrlen) < 0) {
        serverError("Failed to bind to socket.");
    }
    displayConnection(result);
    freeaddrinfo(result);
}

void XOsServer::beginListen() {
    if(listen(m_socket, SOMAXCONN) < 0) {
        serverError("Failed to listen to socket.");
    }
}

void XOsServer::acceptConnections() {

    SOCKET clientSocket{ INVALID_SOCKET };
    clientSocket = accept(m_socket, NULL, NULL);
    char recvbuf[DEFAULT_BUFFER_LENGTH];
    int dataSize;
    int recvbuflen = DEFAULT_BUFFER_LENGTH;

    do {
        dataSize = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (dataSize < 0) {
            serverError("Failed to recieve");
        } else {
            deserializeData(recvbuf);
        }

    } while (dataSize >= 0);
    std::cout << "Connection Closed\n";
}

void XOsServer::deserializeData(char* recvBuffer) {
    XOsRequestType rt = (XOsRequestType)recvBuffer[0];
    int payloadSize = (int) recvBuffer[1];

    switch (rt) {
    case XOsRequestType::JOIN:
        if (m_debug) {
            std::cout << "JOIN ";
        }
        break;
    case XOsRequestType::ACCEPT:
        if (m_debug) {
            std::cout << "ACCEPT ";
        }
        break;
    case XOsRequestType::DISCONNECT:
        if (m_debug) {
            std::cout << "DISCONNECT ";
        }
        break;
    case XOsRequestType::LIST:
        if (m_debug) {
            std::cout << "LIST ";
        }
        break;
    case XOsRequestType::MOVE:
        if (m_debug) {
            std::cout << "MOVE ";
        }
        break;
    case XOsRequestType::CHALLENGE:
        if (m_debug) {
            std::cout << "CHALLENGE ";
        }
        break;
    }
    
    if (m_debug) {
        std::cout << "Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';
    }
}

void XOsServer::serverError(const std::string & errorMessage) {
    std::cout << "ERROR: " << errorMessage << " CODE: " << WSAGetLastError() << '\n';
    closesocket(m_socket);
    WSACleanup();
    exit(1);
}

void XOsServer::displayConnection(addrinfo * addressInfo) {
    //Display IP address / port
    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    if (getnameinfo(addressInfo->ai_addr, addressInfo->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
        serverError("Failed getting connection information.");
    }
    std::cout << "Server opened conenction on ip: " << ip << ":" << port << '\n';
}


