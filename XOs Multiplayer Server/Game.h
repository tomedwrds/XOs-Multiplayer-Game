#pragma once
#include <string.h>
#include <iostream>
#define NO_FLAG 0
#define INVALID_MOVE 1
#define GAME_WON 2

class Game {
public:
	Game(char, char, char);
	char makeMove(char);
	char m_nonPlayerMoving;
	char m_playerMoving;
	
	char m_state[9]{};
private:
	char m_gameId;
	bool checkWin();
};
