#include "Node.h"

void Node::nodePrint()
{
	for (int x = 0; x < BOARD_SIZE + 2; x++)
	{
		for (int y = 0; y < BOARD_SIZE + 2; y++)
		{
			printf(" %d", NodeBoard[y][x]);
		}
		printf("\n");
	}
	printf("\n\n");
}

void Node::update(Board& board)
{
	board.nodeUpdate(*NodeBoard);
}