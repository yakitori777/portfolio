#include "Board.h"

Board::Board()
{
	init();
}

void Board::init()
{
	// 全マスを空きマスに設定
	for(int x=1; x <= BOARD_SIZE; x++)
	{
		for(int y=1; y <= BOARD_SIZE; y++)
		{
			RawBoard[x][y] = EMPTY;
		}
	}
	
	// 壁の設定
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


	// 初期配置
	RawBoard[4][4] = WHITE;
	RawBoard[5][5] = WHITE;
	RawBoard[4][5] = BLACK;
	RawBoard[5][4] = BLACK;
	
	// 開放度の初期設定
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

	// 石数の初期設定
	Discs[BLACK] = 2;
	Discs[WHITE] = 2;
	Discs[EMPTY] = BOARD_SIZE*BOARD_SIZE - 4;

	Turns = 0; // 手数は0から数える
	CurrentColor = BLACK; // 先手は黒
	
	// updateを全て消去
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
	// 打つ手があればパスできない
	if(MovablePos[Turns].size() != 0) return false;
	
	// ゲームが終了しているなら、パスできない
	if(isGameOver()) return false;
	
	CurrentColor = -CurrentColor;
	
	// 空のupdateを挿入しておく
	std::vector<Disc> nullupdate;
	UpdateLog.push_back(nullupdate);
	
	initMovable();
	
	return true;
}

bool Board::undo()
{
	// ゲーム開始地点ならもう戻れない
	if(Turns == 0) return false;

	CurrentColor = -CurrentColor;
	
	const std::vector<Disc> &update = UpdateLog.back();

	// 前回がパスかどうかで場合分け

	if(update.empty()) // 前回はパス
	{
		// MovablePos及びMovableDirを再構築
		MovablePos[Turns].clear();
		for(unsigned x=1; x<=BOARD_SIZE; x++)
		{
			for(unsigned y=1; y<=BOARD_SIZE; y++)
			{
				MovableDir[Turns][x][y] = NONE;
			}
		}
	}
	else // 前回はパスでない
	{
		Turns--;

		int x = update[0].x;
		int y = update[0].y;

		// 石を元に戻す
		RawBoard[x][y] = EMPTY;
		for(unsigned i=1; i<update.size(); i++)
		{
			RawBoard[update[i].x][update[i].y] = -update[i].color;
		}

		// 開放度の更新
		Liberty[x+1][y]--;
		Liberty[x+1][y-1]--;
		Liberty[x][y-1]--;
		Liberty[x-1][y-1]--;
		Liberty[x-1][y]--;
		Liberty[x-1][y+1]--;
		Liberty[x][y+1]--;
		Liberty[x+1][y+1]--;
		
		// 石数の更新
		unsigned discdiff = update.size();
		Discs[CurrentColor] -= discdiff;
		Discs[-CurrentColor] += discdiff -1;
		Discs[EMPTY]--;
	}

	// 不要になったupdateを1つ削除
	UpdateLog.pop_back();

	return true;
}

unsigned Board::checkMobility(const Disc& disc) const
{
	// 既に石があったら置けない
	if(RawBoard[disc.x][disc.y] != EMPTY) return NONE;

	int x, y;
	unsigned dir = NONE;

	// 上
	if(RawBoard[disc.x][disc.y-1] == -disc.color)
	{
		x = disc.x; y = disc.y-2;
		while(RawBoard[x][y] == -disc.color) { y--; }
		if(RawBoard[x][y] == disc.color) dir |= UPPER;
	}

	// 下
	if(RawBoard[disc.x][disc.y+1] == -disc.color)
	{
		x = disc.x; y = disc.y+2;
		while(RawBoard[x][y] == -disc.color) { y++; }
		if(RawBoard[x][y] == disc.color) dir |= LOWER;
	}

	// 左
	if(RawBoard[disc.x-1][disc.y] == -disc.color)
	{
		x = disc.x-2; y = disc.y;
		while(RawBoard[x][y] == -disc.color) { x--; }
		if(RawBoard[x][y] == disc.color) dir |= LEFT;
	}

	// 右
	if(RawBoard[disc.x+1][disc.y] == -disc.color)
	{
		x = disc.x+2; y = disc.y;
		while(RawBoard[x][y] == -disc.color) { x++; }
		if(RawBoard[x][y] == disc.color) dir |= RIGHT;
	}


	// 右上
	if(RawBoard[disc.x+1][disc.y-1] == -disc.color)
	{
		x = disc.x+2; y = disc.y-2;
		while(RawBoard[x][y] == -disc.color) { x++; y--; }
		if(RawBoard[x][y] == disc.color) dir |= UPPER_RIGHT;
	}

	// 左上
	if(RawBoard[disc.x-1][disc.y-1] == -disc.color)
	{
		x = disc.x-2; y = disc.y-2;
		while(RawBoard[x][y] == -disc.color) { x--; y--; }
		if(RawBoard[x][y] == disc.color) dir |= UPPER_LEFT;
	}

	// 左下
	if(RawBoard[disc.x-1][disc.y+1] == -disc.color)
	{
		x = disc.x-2; y = disc.y+2;
		while(RawBoard[x][y] == -disc.color) { x--; y++; }
		if(RawBoard[x][y] == disc.color) dir |= LOWER_LEFT;
	}

	// 右下
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
	// 60手に達していたらゲーム終了
	if(Turns == MAX_TURNS) return true;
	
	// 打てる手があるならゲーム終了ではない
	if(MovablePos[Turns].size() != 0) return false;
	
	//
	//	現在の手番と逆の色が打てるかどうか調べる
	//
	Disc disc;
	disc.color = -CurrentColor;
	for(int x=1; x<=BOARD_SIZE; x++)
	{
		disc.x = x;
		for(int y=1; y<=BOARD_SIZE; y++)
		{
			disc.y = y;
			// 置ける箇所が1つでもあればゲーム終了ではない
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
				// 置ける
				MovablePos[Turns].push_back(disc);
			}
			MovableDir[Turns][x][y] = dir;
		}
	}
}


void Board::flipDiscs(const Point& point)
{
	int x, y;
	Disc operation(point.x , point.y, CurrentColor); // 行った操作を表す石
	int dir = MovableDir[Turns][point.x][point.y];

	std::vector<Disc> update;

	RawBoard[point.x][point.y] = CurrentColor;
	update.push_back(operation);

	// 上

	if(dir & UPPER) // 上に置ける
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


	// 下

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

	// 左

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

	// 右

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

	// 右上

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

	// 左上

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

	// 左下

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

	// 右下

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

	// 開放度を更新
	Liberty[x+1][y]++;
	Liberty[x+1][y-1]++;
	Liberty[x][y-1]++;
	Liberty[x-1][y-1]++;
	Liberty[x-1][y]++;
	Liberty[x-1][y+1]++;
	Liberty[x][y+1]++;
	Liberty[x+1][y+1]++;

	// 石の数を更新

	unsigned discdiff = update.size();

	Discs[CurrentColor]  += discdiff;
	Discs[-CurrentColor] -= discdiff -1;
	Discs[EMPTY]--;
	
	UpdateLog.push_back(update);
}

