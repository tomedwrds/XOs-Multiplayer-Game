#pragma once
#include <string.h>
#include <iostream>
#define INVALID_MOVE 0
#define VALID_MOVE 1
#define GAME_WON 2

class Game {
public:
	Game(char, char, char);
	int makeMove(char);
	char m_nonPlayerMoving;
	char m_playerMoving;

	char m_state[9]{};
private:
	char m_gameId;
};
