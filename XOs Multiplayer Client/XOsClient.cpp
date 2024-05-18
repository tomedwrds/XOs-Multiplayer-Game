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

    //Create server
    struct addrinfo* result, hints, *ptr;
    memset(&hints, 0, sizeof(hints));

    //Assign hints
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    if (getaddrinfo("127.0.0.1", DEFAULTPORT, &hints, &result) < 0) {
        serverError("Failed to listen to get address info.");
    }

    //Create socket
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


void XOsClient::serverError(const std::string& errorMessage) {
    std::cout << "ERROR: " << errorMessage << " CODE: " << WSAGetLastError() << '\n';
    closesocket(m_socket);
    WSACleanup();
    exit(1);
}

void XOsClient::displayConnection(addrinfo* addressInfo) {
    //Display IP address / port
    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    if (getnameinfo(addressInfo->ai_addr, addressInfo->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
        serverError("Failed getting connection information.");
    }
    std::cout << "Server opened conenction on ip: " << ip << ":" << port << '\n';
}

enum XOsRequestType : char {
    JOIN,
    LIST,
    CHALLENGE,
    ACCEPT,
    MOVE,
    DISCONNECT
};


void seralizeData(XOsRequestType rt, char* payload, char payloadSize, int socket) {
    const int headerSize{ 2 };
    char* sendBuffer = new char[payloadSize + headerSize];
    
    sendBuffer[0] = rt;
    sendBuffer[1] = payloadSize;
    
    for (int i = headerSize; i < payloadSize + headerSize; i++) {
        sendBuffer[i] = (char)payload[i - headerSize];
    }
    send(socket, sendBuffer, payloadSize + headerSize, 0);
    delete[] sendBuffer;
}



void XOsClient::sendData() {

    char data[200]{ "hello world" };
    seralizeData(XOsRequestType::ACCEPT, ((char*)data), 20, m_socket);


    int recvbuflen = DEFAULT_BUFFER_LENGTH;

    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFFER_LENGTH];

    int iResult;

    // Send an initial buffer

    iResult = send(m_socket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        serverError("Failed to send data");
    }
    std::cout << WSAGetLastError();
    iResult = shutdown(m_socket, SD_SEND);

    while (1);
}