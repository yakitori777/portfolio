#include "Board.h"

Board::Board()
{
	init();
}

void Board::init()
{
	// �S�}�X���󂫃}�X�ɐݒ�
	for(int x=1; x <= BOARD_SIZE; x++)
	{
		for(int y=1; y <= BOARD_SIZE; y++)
		{
			RawBoard[x][y] = EMPTY;
		}
	}
	
	// �ǂ̐ݒ�
	for(int y=0; y < BOARD_SIZE + 2; y++)
	{
		RawBoard[0][y] = WALL;
		RawBoard[BOARD_SIZE+1][y] = WALL;
	}

	for(int x=0; x < BOARD_SIZE + 2; x++)
	{
		RawBoard[x][0] = WALL;
		RawBoard[x][BOARD_SIZE+1] = WALL;
	}


	// �����z�u
	RawBoard[4][4] = WHITE;
	RawBoard[5][5] = WHITE;
	RawBoard[4][5] = BLACK;
	RawBoard[5][4] = BLACK;
	
	// �J���x�̏����ݒ�
	for(int x=1; x <= BOARD_SIZE; x++)
	{
		for(int y=0; y <= BOARD_SIZE; y++)
		{
			unsigned liberty = 0;
			if(RawBoard[x+1][y] == EMPTY) liberty++;
			if(RawBoard[x+1][y-1] == EMPTY) liberty++;
			if(RawBoard[x][y-1] == EMPTY) liberty++;
			if(RawBoard[x-1][y-1] == EMPTY) liberty++;
			if(RawBoard[x-1][y] == EMPTY) liberty++;
			if(RawBoard[x-1][y+1] == EMPTY) liberty++;
			if(RawBoard[x][y+1] == EMPTY) liberty++;
			if(RawBoard[x+1][y+1] == EMPTY) liberty++;

			Liberty[x][y] = liberty;
		}
	}

	// �ΐ��̏����ݒ�
	Discs[BLACK] = 2;
	Discs[WHITE] = 2;
	Discs[EMPTY] = BOARD_SIZE*BOARD_SIZE - 4;

	Turns = 0; // �萔��0���琔����
	CurrentColor = BLACK; // ���͍�
	
	// update��S�ď���
	UpdateLog.clear();

	initMovable();
}

bool Board::move(const Point& point)
{
	if(point.x <= 0 || point.x > BOARD_SIZE) return false;
	if(point.y <= 0 || point.y > BOARD_SIZE) return false;
	if(MovableDir[Turns][point.x][point.y] == NONE) return false;
	
	flipDiscs(point);

	Turns++;
	CurrentColor = -CurrentColor;

	initMovable();

	return true;
}

bool Board::pass()
{
	// �ł肪����΃p�X�ł��Ȃ�
	if(MovablePos[Turns].size() != 0) return false;
	
	// �Q�[�����I�����Ă���Ȃ�A�p�X�ł��Ȃ�
	if(isGameOver()) return false;
	
	CurrentColor = -CurrentColor;
	
	// ���update��}�����Ă���
	std::vector<Disc> nullupdate;
	UpdateLog.push_back(nullupdate);
	
	initMovable();
	
	return true;
}

bool Board::undo()
{
	// �Q�[���J�n�n�_�Ȃ�����߂�Ȃ�
	if(Turns == 0) return false;

	CurrentColor = -CurrentColor;
	
	const std::vector<Disc> &update = UpdateLog.back();

	// �O�񂪃p�X���ǂ����ŏꍇ����

	if(update.empty()) // �O��̓p�X
	{
		// MovablePos�y��MovableDir���č\�z
		MovablePos[Turns].clear();
		for(unsigned x=1; x<=BOARD_SIZE; x++)
		{
			for(unsigned y=1; y<=BOARD_SIZE; y++)
			{
				MovableDir[Turns][x][y] = NONE;
			}
		}
	}
	else // �O��̓p�X�łȂ�
	{
		Turns--;

		int x = update[0].x;
		int y = update[0].y;

		// �΂����ɖ߂�
		RawBoard[x][y] = EMPTY;
		for(unsigned i=1; i<update.size(); i++)
		{
			RawBoard[update[i].x][update[i].y] = -update[i].color;
		}

		// �J���x�̍X�V
		Liberty[x+1][y]--;
		Liberty[x+1][y-1]--;
		Liberty[x][y-1]--;
		Liberty[x-1][y-1]--;
		Liberty[x-1][y]--;
		Liberty[x-1][y+1]--;
		Liberty[x][y+1]--;
		Liberty[x+1][y+1]--;
		
		// �ΐ��̍X�V
		unsigned discdiff = update.size();
		Discs[CurrentColor] -= discdiff;
		Discs[-CurrentColor] += discdiff -1;
		Discs[EMPTY]--;
	}

	// �s�v�ɂȂ���update��1�폜
	UpdateLog.pop_back();

	return true;
}

unsigned Board::checkMobility(const Disc& disc) const
{
	// ���ɐ΂���������u���Ȃ�
	if(RawBoard[disc.x][disc.y] != EMPTY) return NONE;

	int x, y;
	unsigned dir = NONE;

	// ��
	if(RawBoard[disc.x][disc.y-1] == -disc.color)
	{
		x = disc.x; y = disc.y-2;
		while(RawBoard[x][y] == -disc.color) { y--; }
		if(RawBoard[x][y] == disc.color) dir |= UPPER;
	}

	// ��
	if(RawBoard[disc.x][disc.y+1] == -disc.color)
	{
		x = disc.x; y = disc.y+2;
		while(RawBoard[x][y] == -disc.color) { y++; }
		if(RawBoard[x][y] == disc.color) dir |= LOWER;
	}

	// ��
	if(RawBoard[disc.x-1][disc.y] == -disc.color)
	{
		x = disc.x-2; y = disc.y;
		while(RawBoard[x][y] == -disc.color) { x--; }
		if(RawBoard[x][y] == disc.color) dir |= LEFT;
	}

	// �E
	if(RawBoard[disc.x+1][disc.y] == -disc.color)
	{
		x = disc.x+2; y = disc.y;
		while(RawBoard[x][y] == -disc.color) { x++; }
		if(RawBoard[x][y] == disc.color) dir |= RIGHT;
	}


	// �E��
	if(RawBoard[disc.x+1][disc.y-1] == -disc.color)
	{
		x = disc.x+2; y = disc.y-2;
		while(RawBoard[x][y] == -disc.color) { x++; y--; }
		if(RawBoard[x][y] == disc.color) dir |= UPPER_RIGHT;
	}

	// ����
	if(RawBoard[disc.x-1][disc.y-1] == -disc.color)
	{
		x = disc.x-2; y = disc.y-2;
		while(RawBoard[x][y] == -disc.color) { x--; y--; }
		if(RawBoard[x][y] == disc.color) dir |= UPPER_LEFT;
	}

	// ����
	if(RawBoard[disc.x-1][disc.y+1] == -disc.color)
	{
		x = disc.x-2; y = disc.y+2;
		while(RawBoard[x][y] == -disc.color) { x--; y++; }
		if(RawBoard[x][y] == disc.color) dir |= LOWER_LEFT;
	}

	// �E��
	if(RawBoard[disc.x+1][disc.y+1] == -disc.color)
	{
		x = disc.x+2; y = disc.y+2;
		while(RawBoard[x][y] == -disc.color) { x++; y++; }
		if(RawBoard[x][y] == disc.color) dir |= LOWER_RIGHT;
	}


	return dir;

}

bool Board::isGameOver() const
{
	// 60��ɒB���Ă�����Q�[���I��
	if(Turns == MAX_TURNS) return true;
	
	// �łĂ�肪����Ȃ�Q�[���I���ł͂Ȃ�
	if(MovablePos[Turns].size() != 0) return false;
	
	//
	//	���݂̎�ԂƋt�̐F���łĂ邩�ǂ������ׂ�
	//
	Disc disc;
	disc.color = -CurrentColor;
	for(int x=1; x<=BOARD_SIZE; x++)
	{
		disc.x = x;
		for(int y=1; y<=BOARD_SIZE; y++)
		{
			disc.y = y;
			// �u����ӏ���1�ł�����΃Q�[���I���ł͂Ȃ�
			if(checkMobility(disc) != NONE) return false;
		}
	}
	
	return true;
}



void Board::nodeUpdate(Color color[])
{
	memcpy(color ,RawBoard ,sizeof(RawBoard) );
}

void Board::boardPrint()
{
	for(int x = 0; x < BOARD_SIZE + 2; x++)
	{
		for (int y = 0; y < BOARD_SIZE + 2 ;y++) 
		{
			printf(" %d", RawBoard[y][x]);
		}
		printf("\n");
	}
	printf("\n\n");
}


void Board::initMovable()
{
	Disc disc(0, 0, CurrentColor);

	int dir;

	MovablePos[Turns].clear();

	for(int x=1; x<=BOARD_SIZE; x++)
	{
		disc.x = x;
		for(int y=1; y<=BOARD_SIZE; y++)
		{
			disc.y = y;
			
			dir = checkMobility(disc);
			if(dir != NONE)
			{
				// �u����
				MovablePos[Turns].push_back(disc);
			}
			MovableDir[Turns][x][y] = dir;
		}
	}
}


void Board::flipDiscs(const Point& point)
{
	int x, y;
	Disc operation(point.x , point.y, CurrentColor); // �s���������\����
	int dir = MovableDir[Turns][point.x][point.y];

	std::vector<Disc> update;

	RawBoard[point.x][point.y] = CurrentColor;
	update.push_back(operation);

	// ��

	if(dir & UPPER) // ��ɒu����
	{
		y = point.y;
		operation.x = point.x;
		while(RawBoard[point.x][--y] == -CurrentColor)
		{
			RawBoard[point.x][y] = CurrentColor;
			operation.y = y;
			update.push_back(operation);
		}
	}


	// ��

	if(dir & LOWER)
	{
		y = point.y;
		operation.x = point.x;
		while(RawBoard[point.x][++y] == -CurrentColor)
		{
			RawBoard[point.x][y] = CurrentColor;
			operation.y = y;
			update.push_back(operation);
		}
	}

	// ��

	if(dir & LEFT)
	{
		x = point.x;
		operation.y = point.y;
		while(RawBoard[--x][point.y] == -CurrentColor)
		{
			RawBoard[x][point.y] = CurrentColor;
			operation.x = x;
			update.push_back(operation);
		}
	}

	// �E

	if(dir & RIGHT)
	{
		x = point.x;
		operation.y = point.y;
		while(RawBoard[++x][point.y] == -CurrentColor)
		{
			RawBoard[x][point.y] = CurrentColor;
			operation.x = x;
			update.push_back(operation);
		}
	}

	// �E��

	if(dir & UPPER_RIGHT)
	{
		x = point.x;
		y = point.y;
		while(RawBoard[++x][--y] == -CurrentColor)
		{
			RawBoard[x][y] = CurrentColor;
			operation.x = x;
			operation.y = y;
			update.push_back(operation);
		}
	}

	// ����

	if(dir & UPPER_LEFT)
	{
		x = point.x;
		y = point.y;
		while(RawBoard[--x][--y] == -CurrentColor)
		{
			RawBoard[x][y] = CurrentColor;
			operation.x = x;
			operation.y = y;
			update.push_back(operation);
		}
	}

	// ����

	if(dir & LOWER_LEFT)
	{
		x = point.x;
		y = point.y;
		while(RawBoard[--x][++y] == -CurrentColor)
		{
			RawBoard[x][y] = CurrentColor;
			operation.x = x;
			operation.y = y;
			update.push_back(operation);
		}
	}

	// �E��

	if(dir & LOWER_RIGHT)
	{
		x = point.x;
		y = point.y;
		while(RawBoard[++x][++y] == -CurrentColor)
		{
			RawBoard[x][y] = CurrentColor;
			operation.x = x;
			operation.y = y;
			update.push_back(operation);
		}
	}

	x = point.x;
	y = point.y;

	// �J���x���X�V
	Liberty[x+1][y]++;
	Liberty[x+1][y-1]++;
	Liberty[x][y-1]++;
	Liberty[x-1][y-1]++;
	Liberty[x-1][y]++;
	Liberty[x-1][y+1]++;
	Liberty[x][y+1]++;
	Liberty[x+1][y+1]++;

	// �΂̐����X�V

	unsigned discdiff = update.size();

	Discs[CurrentColor]  += discdiff;
	Discs[-CurrentColor] -= discdiff -1;
	Discs[EMPTY]--;
	
	UpdateLog.push_back(update);
}

