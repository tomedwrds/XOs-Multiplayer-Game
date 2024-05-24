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

void XOsServer::acceptConnection() {
    while (1) {
        SOCKET clientSocket{ INVALID_SOCKET };
        clientSocket = accept(m_socket, NULL, NULL);
        if (clientSocket < 0) {
            serverError("Failed to accept connection");
        }
        std::thread workerThread(&XOsServer::serverActive, this, clientSocket);
        workerThread.detach();
    }
}

void XOsServer::serverActive(int clientSocket) {
    char recvbuf[DEFAULT_BUFFER_LENGTH];
    int dataSize;
    int recvbuflen = DEFAULT_BUFFER_LENGTH;

    do {
        dataSize = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (dataSize < 0) {
            serverError("Failed to recieve");
        } else if (dataSize > 0) {
            deserializeData(recvbuf, clientSocket);
            std::memset(recvbuf, 0, DEFAULT_BUFFER_LENGTH);
        }

    } while (dataSize > 0);
    std::cout << "Connection Closed\n";
    while (1);
}


void XOsServer::deserializeData(char* recvBuffer, int clientSocket) {
    XOsRequestType rt = (XOsRequestType)recvBuffer[0];
    int senderId = (int) recvBuffer[1];
    int payloadSize = (int) recvBuffer[2];
    if (m_debug) {
        outputRequest(recvBuffer);
    }

    switch (rt) {
    case XOsRequestType::JOIN:     
        {
            std::string userName{ recvBuffer + HEADER_SIZE };
            if (m_users.count(userName) == 0) {
                m_users.insert(std::pair<std::string, char>(userName, ++m_totalUsers));
                seralizeAndSendData(XOsRequestType::JOIN, &m_totalUsers, 1, clientSocket);
            }
            else {
                char responseFailed{ JOIN_FAIL };
                seralizeAndSendData(XOsRequestType::JOIN, &responseFailed, 1, clientSocket);
            }
            
        }
        break;
    case XOsRequestType::ACCEPT:
       
        break;
    case XOsRequestType::DISCONNECT:
      
        break;
    case XOsRequestType::LIST:
    {
        char* userNames = new char[(m_users.size()-1) * (USERNAME_LENGTH+1)];

        char namesAdded = 0;
        for (auto const& x : m_users) {
            if (x.second != senderId) {
                userNames[namesAdded * (USERNAME_LENGTH + 1)] = x.second;
                std::copy(x.first.c_str(), x.first.c_str() + USERNAME_LENGTH, userNames + (namesAdded * (USERNAME_LENGTH + 1) + 1));
                namesAdded++;
            }
            
        }
        seralizeAndSendData(XOsRequestType::LIST, userNames, (m_users.size()-1) * (USERNAME_LENGTH + 1), clientSocket);
        delete[] userNames;
    }
        
        break;
    case XOsRequestType::MOVE:
       
        break;
    case XOsRequestType::CHALLENGE:
        char challengedClient = recvBuffer[HEADER_SIZE];
        std::cout << (int)challengedClient;
        if (m_challenges.count(challengedClient)) {
            m_challenges[challengedClient].insert(senderId);

        }
        else {
            std::set<char> currentChallenges { (char)senderId };
            m_challenges.insert(std::make_pair(challengedClient, currentChallenges));
        }
        
        break;
    }
    
    
}

void XOsServer::outputRequest(char* recvBuffer) {
    XOsRequestType rt = (XOsRequestType)recvBuffer[0];
    int senderId = (int)recvBuffer[1];
    int payloadSize = (int)recvBuffer[2];

    switch (rt) {
    case XOsRequestType::JOIN:
        std::cout << "JOIN ";
        break;
    case XOsRequestType::ACCEPT:
        std::cout << "ACCEPT ";
        break;
    case XOsRequestType::DISCONNECT:
        std::cout << "DISCONNECT ";
        break;
    case XOsRequestType::LIST:
        std::cout << "LIST ";
        break;
    case XOsRequestType::MOVE:
        std::cout << "MOVE ";
        break;
    case XOsRequestType::CHALLENGE:
        std::cout << "CHALLENGE ";
        break;
    }

    std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';

}

void XOsServer::seralizeAndSendData(XOsRequestType rt, char* payload, char payloadSize, int clientSocket) {
    char* sendBuffer = new char[payloadSize + HEADER_SIZE];
    sendBuffer[0] = rt;
    sendBuffer[1] = 0xff;
    sendBuffer[2] = payloadSize;

    for (int i = HEADER_SIZE; i < payloadSize + HEADER_SIZE; i++) {
        sendBuffer[i] = (char)payload[i - HEADER_SIZE];
    }
    if (send(clientSocket, sendBuffer, payloadSize + HEADER_SIZE, 0) < 0) {
        serverError("Failed to send data");
    }
    delete[] sendBuffer;
}


void XOsServer::serverError(const std::string & errorMessage) {
    std::cout << "ERROR: " << errorMessage << " CODE: " << WSAGetLastError() << '\n';
    closesocket(m_socket);
    WSACleanup();
    exit(1);
}

void XOsServer::displayConnection(addrinfo * addressInfo) {
    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    if (getnameinfo(addressInfo->ai_addr, addressInfo->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
        serverError("Failed getting connection information.");
    }
    std::cout << "Server opened conenction on ip: " << ip << ":" << port << '\n';
}


