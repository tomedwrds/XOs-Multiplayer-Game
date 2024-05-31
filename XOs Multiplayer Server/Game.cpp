#include "Game.h"

Game::Game(char player1, char player2, char gameId) 
	: m_playerMoving{ player1 }, m_nonPlayerMoving{ player2 }, m_gameId{ gameId } {
}

char Game::makeMove(char movePos) {
	if (m_state[movePos] == 0) {
		
		m_state[movePos] = m_playerMoving;
		if (checkWin()) {
			return GAME_WON;
		}

		char temp{ m_nonPlayerMoving };
		m_nonPlayerMoving = m_playerMoving;
		m_playerMoving = temp;
	}
	else {
		return INVALID_MOVE;
	}
	return NO_FLAG;
}
bool Game::checkWin() {
	//first check all rows
	for (int i = 0; i < 3; i++) {
		if (m_state[i*3] && (m_state[i*3] == m_state[i*3+1]) && (m_state[i*3+1] == m_state[i*3 + 2]))
			return true;
	}
	//then check columns
	for (int i = 0; i < 3; i++) {
		if (m_state[i] && (m_state[i] == m_state[3 + i]) && (m_state[3 + i] == m_state[6 + i]))
			return true;
	}
	//then finally diagonals
	if ((m_state[0] && (m_state[0] == m_state[4]) && (m_state[4] == m_state[8])) || (m_state[2] && (m_state[2] == m_state[4]) && (m_state[4] == m_state[6])))
		return true;
	return false;
	 
}