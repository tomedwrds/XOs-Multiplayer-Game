// XOs Multiplayer Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#define DEFAULTPORT "5208"
#pragma comment(lib, "Ws2_32.lib")


int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        std::cout << "Errror";
    }
    //Create server
    struct addrinfo* result, hints, *ptr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    if (getaddrinfo("127.0.0.1", DEFAULTPORT, &hints, &result) < 0) {
        std::cout << "error";
    }
 

    SOCKET ConnectSocket = INVALID_SOCKET;
    ptr = result;

    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket < 0) {
        std::cout << "error socket";
    }

    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    int a = getnameinfo(result->ai_addr, result->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << "Server opened conenction on ip: " << ip << ":" << port << '\n';

    if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) < 0) {
        std::cout << "error conect " << WSAGetLastError();
    }
    freeaddrinfo(result);


    #define DEFAULT_BUFLEN 512

    int recvbuflen = DEFAULT_BUFLEN;

    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];

    int iResult;

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    iResult = shutdown(ConnectSocket, SD_SEND);

    while (1);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
