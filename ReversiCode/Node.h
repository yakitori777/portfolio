#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED
#include "Board.h"
#include "Reversi.h"
#include <vector>
#include <string>
class Node {


public:
	Color NodeBoard[BOARD_SIZE + 2][BOARD_SIZE + 2]; //ó‘Ô
	int wins = 0; //—İŒv‰¿’l
	int total_try = 0; //s‰ñ”

	void nodePrint();
	void update(Board& board);
};

#endif