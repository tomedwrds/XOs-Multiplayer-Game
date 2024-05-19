#pragma once
#define HEADER_SIZE 2


enum XOsResponseType {
	JOIN_ACCEPT,
	LIST_DATA,
	CHALLENGE_REQUEST,
	MOVE_ACCEPT,
	GAME_STATE
};


enum XOsRequestType : char {
    JOIN,
    LIST,
    CHALLENGE,
    ACCEPT,
    MOVE,
    DISCONNECT
};

