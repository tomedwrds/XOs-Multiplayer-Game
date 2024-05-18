#pragma once
#include <map>
enum XOsRequestType: char{
	JOIN,
	LIST,
	CHALLENGE,
	ACCEPT,
	MOVE,
	DISCONNECT
};


union XOsRequestPayload {
	char userName[8];
	int userChallengeId;
	int acceptChallenge;
	int movePos[2];
};

struct XOsRequest {
	XOsRequestType requestType;
	XOsRequestPayload messagePayload;
};

enum XOsResponseType {
	JOIN_ACCEPT,
	LIST_DATA,
	CHALLENGE_REQUEST,
	MOVE_ACCEPT,
	GAME_STATE
};

union XOsResponsePayload {
	int opponentAccepted;
	char challengerName[8];
	char challengerNameList[6][8];
	int moveValid;
	int gameState[3][3][2];
};

struct XOsResponse {
	XOsResponseType responseType;
	XOsResponsePayload messagePayload;
};