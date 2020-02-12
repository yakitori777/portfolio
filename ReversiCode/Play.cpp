#include "Board.h"
#include <iostream>
#include <list>
#include <random>
#include <time.h>
#include <windows.h>

const int MCTS_PLAY_COUNT = 200;
const int PRIMAL_MCTS_PLAY_COUNT = 50;

class NodeBoard;

int mcts(Board board);
int mctsV2(Board board);
double ucb1(int wins, int total_try, int all_total_try);
int aiBattle(Board board);
int autoBattle(int *black_num, int *white_num,int *draw_num);
int randomIndex(int max);
double winRate(NodeBoard board);
using namespace std;

/*
	ボードクラスを継承し、盤面を出力するprint関数を追加
*/


class NodeBoard : public Board
{
public:
	NodeBoard()
	{
		Color nodeBoard[BOARD_SIZE + 2][BOARD_SIZE + 2] = {};
		wins = 0;
		total_try = 0;
		total = 0;
	}

	void nodeBoardPrint()
	{
		for (int x = 0; x < BOARD_SIZE + 2; x++)
		{
			for (int y = 0; y < BOARD_SIZE + 2; y++)
			{
				printf(" %d", nodeBoard[y][x]);
			}
			printf("\n");
		}
		printf("\n\n");
	}

	int getWins()
	{
		return this->wins;
	}

	int getTotalTry()
	{
		return total_try;
	}

	int getTotal()
	{
		return this->total_try;
	}

	void addWins(int num)
	{
		this->wins = this->wins + (num);
	}

	void addTotalTry(int num1)
	{
		this->total_try = this->total_try + (num1);
	}

	void addTotal(int num1)
	{
		this->total = this->total + (num1);
	}

	void update(Board& board)
	{
		board.nodeUpdate(*nodeBoard);
	}

	void print()
	{
		cout << "  a b c d e f g h " << endl;
		for (int y = 1; y <= 8; y++)
		{
			cout << " " << y;
			for (int x = 1; x <= 8; x++)
			{
				switch (getColor(Point(x, y)))
				{
				case BLACK:
					cout << "●";
					break;
				case WHITE:
					cout << "○";
					break;
				default:
					cout << "　";
					break;
				}
			}
			cout << endl;
		}
	}
private:
	Color nodeBoard[BOARD_SIZE + 2][BOARD_SIZE + 2]; //状態
	int wins; //累計価値
	int total_try; //試行回数
	int total;
};

ostream& operator<<(ostream& os, const Point& p)
{
	string s = p;
	os << s;
	return os;
}

int aiBattle(Board board)
{
	while (true)
	{
		vector<Point> movables = board.getMovablePos(); //合法手を取得
		Point p;
		string in;
		if (movables.size() >= 2)
		{

			in = movables[randomIndex(movables.size() - 1)];
		}
		if (movables.size() == 1)
		{
			in = movables[0];
		}
		if (movables.size() == 0)
		{
			in = "p";
		}
		if (in == "p")
		{
			if (!board.pass()) {
				continue;
			}
		}
		if (in == "u")
		{
			board.undo();
			continue;
		}
		try
		{
			Point parse(in);
			p = parse;
		}
		catch (invalid_argument e)
		{
			continue;
		}
		if (board.move(p) == false)
		{
			continue;
		}
		if (board.isGameOver())
		{
			if (board.countDisc(BLACK) == board.countDisc(WHITE))
			{
				return 0;
			}
			else if (board.countDisc(BLACK) > board.countDisc(WHITE))
			{
				return 1;
			}
			else {
				return -1;
			}
		}
	}
}


int autoBattle(int *black_num,int *white_num,int *draw_num)
{
	NodeBoard board;
	while (true)
	{
		//合法手を取得
		vector<Point> movables = board.getMovablePos();
		Point p;
		string in;
		if (movables.size() >= 2)
		{
			in = movables[randomIndex(movables.size() - 1)];
		}
		if (movables.size() == 1)
		{
			in = movables[0];
		}
		if (movables.size() == 0)
		{
			in = "p";
		}
		if (in == "p")
		{
			// パス
			if (!board.pass()) {
				continue;
			}
		}

		if (in == "u")
		{
			board.undo();
			continue;
		}
		try
		{
			Point parse(in);
			p = parse;
		}
		catch (invalid_argument e)
		{
			continue;
		}
		if (board.move(p) == false)
		{
			continue;
		}
		if (board.isGameOver())
		{
			if (board.countDisc(BLACK) == board.countDisc(WHITE))
			{
				*draw_num = *draw_num + 1;
			}
			else if (board.countDisc(BLACK) > board.countDisc(WHITE))
			{
				*black_num=*black_num + 1;
			}
			else {
				*white_num = *white_num + 1;
			}
			return 0;
		}
	}
}

int mctsV2(Board board)
{
	vector<Point> movables = board.getMovablePos(); //合法手を取得
	int size = movables.size();
	vector<NodeBoard> node_list(size);
	NodeBoard temp;
	int max_index = 0;
	double max_ucb1 = 0;
	int turns = board.getTurns();
	/*ノードリストを作成*/
	for (int i = 0; i < size; i++)
	{
		node_list[i].update(board);

		Point p;
		string in = movables[i];
		try
		{

			Point parse(in);
			p = parse;
		}
		catch (invalid_argument e) {
			continue;
		}

		if (node_list[i].move(p) == false)
		{
			continue;
		}
	}
	for (int j = 0; j < size ;j++) 
	{
		for (int i = 0; i < PRIMAL_MCTS_PLAY_COUNT; i++)
		{
			max_index = 0;
			max_ucb1 = 0;
			int judge = aiBattle(node_list[j]);
			if (board.getCurrentColor() == WHITE) {//白なら
				node_list[j].addWins(-judge);
				node_list[j].addTotalTry(1);
			}
			else {//黒なら
				node_list[j].addWins(judge);
				node_list[j].addTotalTry(1);
			}
		}
	}
	double ucb1_temp = 0.0;
	max_ucb1 = -PRIMAL_MCTS_PLAY_COUNT;
	max_index = 0;
	for (int j = 0; j < size; j++)
	{
		if (node_list[j].getWins() > max_ucb1) {
			max_ucb1 = node_list[j].getWins();
			max_index = j;
		}
		//cout << movables[j] << "の勝ち数：" << node_list[j].getWins() << "   ucb1：" << ucb1_temp << "   トータルトライ：" << node_list[j].getTotal() << "   トータルトライ：" << node_list[j].getTotal() << endl;

	}
	return max_index;
}



int mcts(Board board) 
{
	vector<Point> movables = board.getMovablePos(); //合法手を取得
	int size = movables.size();
	vector<NodeBoard> node_list(size);
	NodeBoard temp;
	int max_index=0;
	double max_ucb1=0;
	int turns = board.getTurns();
	//cout << size << endl;
	/*ノードリストを作成*/
	for (int i = 0; i < size; i++) 
	{
		node_list[i].update(board);

		Point p;
		string in = movables[i];
		try
		{

			Point parse(in);
			p = parse;
		}catch (invalid_argument e){
			continue;
		}
		if (node_list[i].move(p) == false)
		{
			continue;
		}
	}
	for(int i=0; i< MCTS_PLAY_COUNT; i++)
	{	
		max_index = 0;
		max_ucb1 = 0;
		/*プレイアウトさせるノードを選択する*/
		for (int j=0; j < size;j++)
		{	
			if (node_list[j].getTotal() == 0) {
				max_index = j;
				break;
			}
			
			if (ucb1(node_list[j].getWins(),node_list[j].getTotal(), i) > max_ucb1){
				max_ucb1 = ucb1(node_list[j].getWins(), node_list[j].getTotal(), i);
				max_index = j;
			}
		}
		int judge = aiBattle(node_list[max_index]);
		int num = 1;
		if(board.getCurrentColor() == WHITE){//白なら
			node_list[max_index].addWins(-judge);
			node_list[max_index].addTotalTry(1);
		}else {//黒なら
			node_list[max_index].addWins(judge);
			node_list[max_index].addTotalTry(1);
		}
	}
	double ucb1_temp = 0.0;
	max_ucb1 = 0;
	max_index = 0;
	for (int j = 0 ; j < size; j++)
	{
		ucb1_temp = winRate(node_list[j]);
		if (ucb1_temp > max_ucb1) {
			max_ucb1 = ucb1_temp;
			max_index = j;
		}
		//cout << movables[j] <<"の勝ち数："<< node_list[j].getWins() << "   ucb1：" << ucb1_temp<<"   トータルトライ：" << node_list[j].getTotal() << "   トータルトライ：" << node_list[j].getTotal() << endl;
		
	}
	return max_index;
}

double ucb1(int wins,int total_try,int all_total_try)
{
	double ucb1_result = 0;
	if (total_try == 0) {
		ucb1_result = 0;
	}else {
		ucb1_result = (wins / (double)total_try) + sqrt(((2 * log(all_total_try)) / total_try));
	}
	return ucb1_result;
}

double winRate(NodeBoard board)
{
	double rate = 0;
	
	rate = (board.getWins() / (double)board.getTotal());

	return rate;
}


int randomIndex(int max) 
{

	return rand() % max;
}


int main()
{	
	/*時間計測用
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	LARGE_INTEGER start, end;
	QueryPerformanceCounter(&start);
	*/
	srand((unsigned int)time(NULL));

	int black=0;
	int white=0;

		NodeBoard board;
		while (true)
		{
			board.print();
			cout << "黒石" << board.countDisc(BLACK) << " ";
			cout << "白石" << board.countDisc(WHITE) << " ";
			cout << "空マス" << board.countDisc(EMPTY) << endl;
			//cout << board.getCurrentColor() == BLACK ? "黒手番 " : "白手番 ";
			//合法手を取得
			vector<Point> movables = board.getMovablePos();
			//合法手を表示
			cout << "候補手 " << "";
			for (unsigned i = 0; i < movables.size(); i++)
			{
				cout << movables[i] << " ";
			}
			cout << endl << endl;

			Point p;
			string in;
			if (board.getTurns() % 2 == 0) {
				/*プレイヤーの手番*/
				cout << "白の番です " << endl;
				cout << "手を入力して下さい: ";
				cin >> in;
				if (movables.size() == 0)
				{
					in = "p";
					cout << "置く場所が無いのでパスします。";
				}
				cout << "白は：" << in << "に打ちました！" << endl;
				if (in == "p")
				{
					// パス
					if (!board.pass()) {
						//cerr << board.getTurns() << endl;
						cerr << "パスできません！" << endl;

						continue;
					}
				}
				if (in == "u")
				{
					// undo
					board.undo();
					continue;
				}
				try
				{

					Point parse(in);
					p = parse;
				}
				catch (invalid_argument e)
				{
					cerr << "リバーシ形式の手を入力して下さい！" << endl;
					continue;
				}
				if (board.move(p) == false)
				{
					cerr << "そこには置けません！" << endl;
					continue;
				}
			}else{
				cout << "黒の番です " << endl;
				if (movables.size() >= 2)
				{
					/*原始モンテカルロ法で候補手を取得*/
					int index = mctsV2(board);
					in = movables[index];
				}
				if (movables.size() == 1)
				{
					in = movables[0];
				}
				if (movables.size() == 0)
				{
					in = "p";
				}
				cout << "黒は：" << in << "に打ちました！" << endl;
				if (in == "p")
				{
					// パス
					if (!board.pass()) {
						//cerr << board.getTurns() << endl;
						cerr << "パスできません！" << endl;

						continue;
					}
				}
				if (in == "u")
				{
					// undo
					board.undo();
					continue;
				}
				try
				{

					Point parse(in);
					p = parse;
				}
				catch (invalid_argument e)
				{
					cerr << "リバーシ形式の手を入力して下さい！" << endl;
					continue;
				}

				if (board.move(p) == false)
				{
					cerr << "そこには置けません！" << endl;
					continue;
				}
			}
			if (board.isGameOver())
			{

				cout << "----------------ゲーム終了----------------" << endl;
				cout << "黒石" << board.countDisc(BLACK) << " ";
				cout << "白石" << board.countDisc(WHITE) << " ";
				if (board.countDisc(BLACK) == board.countDisc(WHITE))
				{
					cout << "引き分け！" << endl;
				}
				else if (board.countDisc(BLACK) > board.countDisc(WHITE))
				{
					cout << "黒の勝ちです！" << endl;
				}
				else {
					cout << "白の勝ちです！" << endl;
				}
				break;
			}
		}

	cout << "白石：" << white << endl;
	cout << "黒石：" << black << endl;

	printf("終わり\n");
	/*時間計測用
	QueryPerformanceCounter(&end);
	double time = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
	printf("time %lf[ms]\n", time);
	*/
	return 0;
}