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
        clientError("Failed WSA Startup");
    }

    struct addrinfo* result, hints, *ptr;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    if (getaddrinfo("127.0.0.1", DEFAULTPORT, &hints, &result) < 0) {
        clientError("Failed to listen to get address info.");
    }

    ptr = result;
    m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (m_socket == INVALID_SOCKET) {
        clientError("Failed to listen to socket.");
    }

    if (connect(m_socket, result->ai_addr, (int)result->ai_addrlen) < 0) {
        clientError("Failed to bind to connect.");
    }
    displayConnection(result);
    freeaddrinfo(result);
}


void XOsClient::clientActive() {
    while (1) {
        std::system("CLS");
        std::cout << "=====================================================\n";
        std::cout << m_userName << " select one of the following\n";
        std::cout << "(1) View other players and request to challenge them.\n";
        std::cout << "(2) View players that have challenged you.\n";
        std::cout << "(3) Exit\n";
        std::cout << "=====================================================\n";
        int selectedOption;
        std::cin >> selectedOption;
        if (selectedOption == 1) {

        }
        else if (selectedOption == 2) {

        }
        else if (selectedOption == 3) {

        }
        
    }


    if (shutdown(m_socket, SD_SEND) < 0) {
        clientError("Failed to shutdown");
    }

    while (1);
}

void XOsClient::clientJoin() {
    std::cout << "Enter your user name to continue \n";
    std::cin >> m_userName;
    seralizeAndSendData(XOsRequestType::JOIN, (char*)m_userName.c_str(), (char)strlen(m_userName.c_str()) + 1);

    char recvbuf[DEFAULT_BUFFER_LENGTH];
    int recvbuflen = DEFAULT_BUFFER_LENGTH;

    if (recv(m_socket, recvbuf, recvbuflen, 0) < 0) {
        clientError("Failed to recieve");
    }
    deserializeData(recvbuf);
}

void XOsClient::seralizeAndSendData(XOsRequestType rt, char* payload, char payloadSize) {
    char* sendBuffer = new char[payloadSize + HEADER_SIZE];
    sendBuffer[0] = rt;
    sendBuffer[1] = payloadSize;
    
    for (int i = HEADER_SIZE; i < payloadSize + HEADER_SIZE; i++) {
        sendBuffer[i] = (char)payload[i - HEADER_SIZE];
    }
    if (send(m_socket, sendBuffer, payloadSize + HEADER_SIZE, 0) < 0) {
        clientError("Failed to send data");
    }
    delete[] sendBuffer;
}


void XOsClient::deserializeData(char* recvBuffer) {
    XOsRequestType rt = (XOsRequestType)recvBuffer[0];
    int payloadSize = (int)recvBuffer[1];
    if (m_debug) {
        outputRequest(recvBuffer);
    }
    switch (rt) {
    case XOsRequestType::JOIN:
        if ((int)recvBuffer[2] == JOIN_FAIL) {
            std::cout << "Username in use please try again\n";
            clientJoin();
        }
        else {
            m_id = (int)recvBuffer[2];
            std::cout << "You have succesfully connected to the server " << m_userName << ".\n";
        }
        break;
    case XOsRequestType::ACCEPT:
        break;
    case XOsRequestType::DISCONNECT:   
        break;
    case XOsRequestType::LIST:   
        break;
    case XOsRequestType::MOVE:     
        break;
    case XOsRequestType::CHALLENGE:
        break;
    }
}

void XOsClient::outputRequest(char* recvBuffer) {
    XOsRequestType rt = (XOsRequestType)recvBuffer[0];
    int payloadSize = (int)recvBuffer[1];

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
    std::cout << "Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';
    
}


void XOsClient::clientError(const std::string& errorMessage) {
    std::cout << "ERROR: " << errorMessage << " CODE: " << WSAGetLastError() << '\n';
    closesocket(m_socket);
    WSACleanup();
    exit(1);
}

void XOsClient::displayConnection(addrinfo* addressInfo) {
    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    if (getnameinfo(addressInfo->ai_addr, addressInfo->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
        clientError("Failed getting connection information.");
    }
    std::cout << "Client joined conenction on ip: " << ip << ":" << port << '\n';
}