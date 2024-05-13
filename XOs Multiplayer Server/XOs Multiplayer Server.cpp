// XOs Multiplayer Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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
    struct addrinfo* result, hints;
    memset(&hints, 0, sizeof(hints));
    //ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    if (getaddrinfo(NULL, DEFAULTPORT, &hints, &result) < 0) {
        std::cout << "error";
    }
    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    int a = getnameinfo(result->ai_addr, result->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
    

    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    
    bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    std::cout << "Server opened conenction on ip: " << ip << ":" << port << '\n';

    freeaddrinfo(result);
    listen(ListenSocket, 10);
    SOCKET ClientSocket;
    ClientSocket = accept(ListenSocket, NULL, NULL);
#define DEFAULT_BUFLEN 512

    char recvbuf[DEFAULT_BUFLEN];
    int iResult, iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);



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
