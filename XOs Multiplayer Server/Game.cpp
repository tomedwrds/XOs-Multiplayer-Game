#include "Game.h"

Game::Game(char player1, char player2, char gameId) 
	: m_playerMoving{ player1 }, m_nonPlayerMoving{ player2 }, m_gameId{ gameId } {
}

int Game::makeMove(char movePos) {
	if (m_state[movePos] != 0) {
		
		m_state[movePos] = m_playerMoving;
		//add game win state logic here
		char temp{ m_playerMoving };
		m_nonPlayerMoving = m_playerMoving;
		m_playerMoving = temp;
	}
	else {
		return INVALID_MOVE;
	}
}