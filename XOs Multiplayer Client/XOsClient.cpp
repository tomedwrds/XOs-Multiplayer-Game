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
        if (m_state == CLIENT_IDLE) {
            if (!m_debug) {
                std::system("CLS");
            }
            std::cout << "=====================================================\n";
            std::cout << m_userName << " select one of the following\n";
            std::cout << "(1) View other players and request to challenge them.\n";
            std::cout << "(2) View players that have challenged you.\n";
            std::cout << "(3) Exit\n";
            std::cout << "=====================================================\n";
            int selectedOption;
            std::cin >> selectedOption;
            if (selectedOption == 1) {
                seralizeAndSendData(XOsRequestType::LIST, (char*)&m_id, 1);
                char recvbuf[DEFAULT_BUFFER_LENGTH];
                int recvbuflen = DEFAULT_BUFFER_LENGTH;

                if (recv(m_socket, recvbuf, recvbuflen, 0) < 0) {
                    clientError("Failed to recieve");
                }
                deserializeData(recvbuf);
            }
            else if (selectedOption == 2) {
                seralizeAndSendData(XOsRequestType::CHALLENGERS, (char*)&m_id, 1);
                char recvbuf[DEFAULT_BUFFER_LENGTH];
                int recvbuflen = DEFAULT_BUFFER_LENGTH;

                if (recv(m_socket, recvbuf, recvbuflen, 0) < 0) {
                    clientError("Failed to recieve");
                }
                deserializeData(recvbuf);
            }
            else if (selectedOption == 3) {

            }
        }
        else if (m_state == CLIENT_WAITING) {
            std::cout << "WAITING..." << "\n";
            char recvbuf[DEFAULT_BUFFER_LENGTH];
            int recvbuflen = DEFAULT_BUFFER_LENGTH;

            if (recv(m_socket, recvbuf, recvbuflen, 0) < 0) {
                clientError("Failed to recieve");
            }
            deserializeData(recvbuf);
        }
        else if (m_state == CLIENT_INGAME) {
            int x{}, y{};
            do {
                std::cout << "Please enter a valid x position of your move (0-2)\n";
                std::cin >> x;
            } while (x < 0 || x > 3);
            do {
                std::cout << "Please enter a valid y position of your move (0-2)\n";
                std::cin >> y;
            } while (y < 0 || y > 3);
            char moveTransformed = y * 3 + x;
            char movePos[2] = {m_currentGame, moveTransformed };
            seralizeAndSendData(XOsRequestType::MOVE, movePos, 2);
            m_state = CLIENT_WAITING;
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
    std::memset(sendBuffer, 0, payloadSize + HEADER_SIZE);
    sendBuffer[0] = rt;
    if (m_id == -1) {
        sendBuffer[1] = 0;
    }
    else {
        sendBuffer[1] = (char)m_id;
    }
    sendBuffer[2] = payloadSize;
    

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
    int senderId = (int)recvBuffer[1];
    int payloadSize = (int)recvBuffer[2];
    if (m_debug) {
        outputRequest(recvBuffer);
    }
    switch (rt) {
    case XOsRequestType::JOIN:
        if ((int)recvBuffer[HEADER_SIZE] == JOIN_FAIL) {
            std::cout << "Username in use please try again\n";
            clientJoin();
        }
        else {
            m_id = recvBuffer[HEADER_SIZE];
            std::cout << "You have succesfully connected to the server " << m_userName << ".\n";
        }
        break;
    case XOsRequestType::CHALLENGERS:
        if (payloadSize > 0) {
            char* idMappings = new char[payloadSize / 16 + 1];
            //create list to store mappings between display order and ID
            for (int i = 0; i * 16 < payloadSize; i++) {
                idMappings[i] = recvBuffer[HEADER_SIZE + i * 16];
            }
            int challengedUser{ -1 };
            do {
                if (!m_debug) {
                    std::system("CLS");
                }
                std::cout << "=====================================================\n";
                std::cout << m_userName << " select one of the following to challenge\n";
                for (int i = 0; i * 16 < payloadSize; i++) {
                    std::cout << "(" << i + 1 << ")" << std::string(recvBuffer + HEADER_SIZE + i * 16 + 1, recvBuffer + HEADER_SIZE + (i + 1) * 16) << "\n";
                }
                std::cout << "(-1) to go back\n";
                std::cout << "=====================================================\n";
                std::cin >> challengedUser;

            } while (challengedUser < -1 || challengedUser >(payloadSize / 16));

            if (challengedUser != -1) {
                char remapepdId = idMappings[challengedUser - 1];
                seralizeAndSendData(XOsRequestType::CHALLENGE, &remapepdId, 1);
                m_state = CLIENT_WAITING;

            }

            delete[] idMappings;
        }
        else {
            std::cout << "=====================================================\n";
            std::cout << "No oponnets online to challenge\n";
            std::cout << "=====================================================\n";
        }
        break;
    case XOsRequestType::GAME_STATE: {
        std::system("CLS");
        m_state = CLIENT_INGAME;
        m_currentGame = recvBuffer[HEADER_SIZE];
        char moveFlag{ recvBuffer[HEADER_SIZE + 1] };
        std::cout << "=====================================================\n";
        if (moveFlag == INVALID_MOVE)
            std::cout << "MOVE INVALID: PLEASE TRY AGAIN\n";
        if (moveFlag == GAME_WON) {
            std::cout << "GAME WON CONGRATS!\n";
            std::system("pause");
            m_state = CLIENT_IDLE;
        }
        if (moveFlag == GAME_DRAW) {
            std::cout << "GAME DRAWN\n";
            std::system("pause");
            m_state = CLIENT_IDLE;
        }
        if (moveFlag == GAME_LOST) {
            std::cout << "GAME LOST HARD LUCK!\n";
            std::system("pause");
            m_state = CLIENT_IDLE;
        }
        std::cout << "Game state\n";
        std::cout << formatMove(recvBuffer[HEADER_SIZE + 2]) << '|' << formatMove(recvBuffer[HEADER_SIZE + 3]) << '|' << formatMove(recvBuffer[HEADER_SIZE + 4]) << '\n';
        std::cout << "-|-|-\n";
        std::cout << formatMove(recvBuffer[HEADER_SIZE + 5]) << '|' << formatMove(recvBuffer[HEADER_SIZE + 6]) << '|' << formatMove(recvBuffer[HEADER_SIZE + 7]) << '\n';
        std::cout << "-|-|-\n";
        std::cout << formatMove(recvBuffer[HEADER_SIZE + 8]) << '|' << formatMove(recvBuffer[HEADER_SIZE + 9]) << '|' << formatMove(recvBuffer[HEADER_SIZE + 10]) << '\n';
        std::cout << "=====================================================\n";
        break;
    }
    case XOsRequestType::LIST:   
    
        if (payloadSize > 0) {
            char* idMappings = new char[payloadSize / 16 + 1];
            //create list to store mappings between display order and ID
            for (int i = 0; i * 16 < payloadSize; i++) {
                idMappings[i] = recvBuffer[HEADER_SIZE + i * 16];
            }
            int challengedUser{ -1 };
            do {
                if (!m_debug) {
                    std::system("CLS");
                }
                std::cout << "=====================================================\n";
                std::cout << m_userName << " select one of the following to challenge\n";
                for (int i = 0; i * 16 < payloadSize; i++) {
                    std::cout << "(" << i + 1 << ")" << std::string(recvBuffer + HEADER_SIZE + i * 16 + 1, recvBuffer + HEADER_SIZE + (i + 1) * 16) << "\n";
                }
                std::cout << "(-1) to go back\n";
                std::cout << "=====================================================\n";
                std::cin >> challengedUser;

            } while (challengedUser < -1 || challengedUser >(payloadSize / 16));

            if (challengedUser != -1) {
                char remapepdId = idMappings[challengedUser - 1];
                seralizeAndSendData(XOsRequestType::CHALLENGE, &remapepdId, 1);
                m_state = CLIENT_WAITING;

            }

            delete[] idMappings;
        }
        else {
            std::cout << "=====================================================\n";
            std::cout << "No oponnets online to challenge\n";
            std::cout << "=====================================================\n";
        }
    
       
        break;
    case XOsRequestType::MOVE:     
        break;
    case XOsRequestType::CHALLENGE:
        break;
    }
}

void XOsClient::outputRequest(char* recvBuffer) {
    XOsRequestType rt = (XOsRequestType)recvBuffer[0];
    int senderId = (int)recvBuffer[1];
    int payloadSize = (int)recvBuffer[2];
    switch (rt) {
    case XOsRequestType::JOIN:
            std::cout << "JOIN ";
            std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (int) recvBuffer[HEADER_SIZE] << '\n';
        break;
    case XOsRequestType::CHALLENGERS:
            std::cout << "CHALLENGERS ";
            std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';
        break;
    case XOsRequestType::DISCONNECT:
            std::cout << "DISCONNECT ";
            std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';

        break;
    case XOsRequestType::LIST:
            std::cout << "LIST ";
            std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';

        break;
    case XOsRequestType::MOVE:
            std::cout << "MOVE ";
            std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';

        break;
    case XOsRequestType::CHALLENGE:
         std::cout << "CHALLENGE ";
         std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';

        break;
    }
    
}

char XOsClient::formatMove(char state) {
    if (state == 0) {
        return ' ';
    } 
    else if (state == m_id) {
        return 'X';
    }
    else {
        return 'O';
    }
}


void XOsClient::clientError(const std::string& errorMessage) {
    std::cout << "ERROR: " << errorMessage << " CODE: " << WSAGetLastError() << '\n';
    closesocket(m_socket);
    WSACleanup();
    exit(1);
}

void XOsClient::closeClient() {
    closesocket(m_socket);
    WSACleanup();
    exit(0);
}

void XOsClient::displayConnection(addrinfo* addressInfo) {
    char ip[NI_MAXHOST];
    char port[NI_MAXSERV];
    if (getnameinfo(addressInfo->ai_addr, addressInfo->ai_addrlen, ip, sizeof(ip), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
        clientError("Failed getting connection information.");
    }
    std::cout << "Client joined conenction on ip: " << ip << ":" << port << '\n';
}