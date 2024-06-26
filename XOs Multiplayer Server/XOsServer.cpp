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
    case XOsRequestType::CHALLENGERS:
    {
        //get the list of challengers
        std::map<char, char> challengers = m_challenges[senderId];
        char* userNames = new char[(challengers.size() * (USERNAME_LENGTH + 1))];

        char namesAdded = 0;
        for (auto const& x : m_users) {
            if (challengers.count(x.second)) {
                userNames[namesAdded * (USERNAME_LENGTH + 1)] = x.second;
                std::copy(x.first.c_str(), x.first.c_str() + USERNAME_LENGTH, userNames + (namesAdded * (USERNAME_LENGTH + 1) + 1));
                namesAdded++;
            }

        }
        seralizeAndSendData(XOsRequestType::LIST, userNames, (m_users.size() - 1) * (USERNAME_LENGTH + 1), clientSocket);
        delete[] userNames;
    }
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
    {
        char gameId = recvBuffer[HEADER_SIZE];
        Game &currentGame = m_games.at(gameId);
        char moveFlag{ currentGame.makeMove(recvBuffer[HEADER_SIZE + 1]) };

        //store game_id if first move
        if (m_challenges[senderId].count(currentGame.m_playerMoving)) {
            m_challenges[senderId][currentGame.m_playerMoving] = gameId;
        }

        //hold execution if not current player
        while (senderId == currentGame.m_nonPlayerMoving);
        char gameData[11];
        gameData[0] = gameId;
        gameData[1] = moveFlag;
        char whoWon = currentGame.checkWin();
        if (whoWon) {
            gameData[1] = whoWon;
        }
       
        memcpy(gameData + 2, currentGame.m_state, 9);

        seralizeAndSendData(XOsRequestType::GAME_STATE, gameData, sizeof(gameData), clientSocket);
    }
        

        break;
    case XOsRequestType::CHALLENGE:
        //check if responding to a challenge or instiating one
        char challengedClient = recvBuffer[HEADER_SIZE];

        //check for in case of response to request
        if (m_challenges.count(senderId)) {
            if (m_challenges[senderId].count(challengedClient)) {
                //create game instance
                Game newGame = Game(senderId, challengedClient, m_gameId);
                m_games.insert(std::make_pair(m_gameId, newGame));

                char gameData[11];
                gameData[0] = m_gameId++;
                gameData[1] = NO_FLAG;
                memcpy(gameData + 2, newGame.m_state, 9);

                seralizeAndSendData(XOsRequestType::GAME_STATE, gameData, sizeof(gameData), clientSocket);
                break;
            }
        }
        if (m_challenges.count(challengedClient)) {
            if (!m_challenges[challengedClient].count(senderId)) {
                m_challenges[challengedClient].insert(std::make_pair(senderId, false));
            }
        }
        else {
            std::map<char, char> currentChallenges;
            currentChallenges.insert(std::make_pair(senderId, 0));
            m_challenges.insert(std::make_pair(challengedClient, currentChallenges));
        }
        //wait for challnege to be resolved by other client
        while (!m_challenges[challengedClient][senderId]);
        Game& currentGame = m_games.at(m_challenges[challengedClient][senderId]);
        char gameData[11];
        gameData[0] = m_challenges[challengedClient][senderId];
        gameData[1] = NO_FLAG;
        memcpy(gameData + 2, currentGame.m_state, 9);

        seralizeAndSendData(XOsRequestType::GAME_STATE, gameData, sizeof(gameData), clientSocket);



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
        std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';

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
        std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << '\n';

        break;
    case XOsRequestType::MOVE:
        std::cout << "MOVE ";
        std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (recvBuffer + HEADER_SIZE) << '\n';

        break;
    case XOsRequestType::CHALLENGE:
        std::cout << "CHALLENGE ";
        std::cout << "Sender:" << senderId << " Size:" << payloadSize << " Message:" << (int) recvBuffer[HEADER_SIZE] << '\n';

        break;
    }


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


