#pragma once
#include <string.h>
#include <iostream>
#define NO_FLAG 0
#define INVALID_MOVE 1
#define GAME_WON 2
#define GAME_LOST 3
#define GAME_DRAW 4

class Game {
public:
	Game(char, char, char);
	char makeMove(char);
	char m_nonPlayerMoving;
	char m_playerMoving;
	char m_state[9]{};
	char checkWin();

private:
	void swapPlayerMoving();
	char m_gameId;
};
