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
	�{�[�h�N���X���p�����A�Ֆʂ��o�͂���print�֐���ǉ�
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
					cout << "��";
					break;
				case WHITE:
					cout << "��";
					break;
				default:
					cout << "�@";
					break;
				}
			}
			cout << endl;
		}
	}
private:
	Color nodeBoard[BOARD_SIZE + 2][BOARD_SIZE + 2]; //���
	int wins; //�݌v���l
	int total_try; //���s��
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
		vector<Point> movables = board.getMovablePos(); //���@����擾
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
		//���@����擾
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
			// �p�X
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
	vector<Point> movables = board.getMovablePos(); //���@����擾
	int size = movables.size();
	vector<NodeBoard> node_list(size);
	NodeBoard temp;
	int max_index = 0;
	double max_ucb1 = 0;
	int turns = board.getTurns();
	/*�m�[�h���X�g���쐬*/
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
			if (board.getCurrentColor() == WHITE) {//���Ȃ�
				node_list[j].addWins(-judge);
				node_list[j].addTotalTry(1);
			}
			else {//���Ȃ�
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
		//cout << movables[j] << "�̏������F" << node_list[j].getWins() << "   ucb1�F" << ucb1_temp << "   �g�[�^���g���C�F" << node_list[j].getTotal() << "   �g�[�^���g���C�F" << node_list[j].getTotal() << endl;

	}
	return max_index;
}



int mcts(Board board) 
{
	vector<Point> movables = board.getMovablePos(); //���@����擾
	int size = movables.size();
	vector<NodeBoard> node_list(size);
	NodeBoard temp;
	int max_index=0;
	double max_ucb1=0;
	int turns = board.getTurns();
	//cout << size << endl;
	/*�m�[�h���X�g���쐬*/
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
		/*�v���C�A�E�g������m�[�h��I������*/
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
		if(board.getCurrentColor() == WHITE){//���Ȃ�
			node_list[max_index].addWins(-judge);
			node_list[max_index].addTotalTry(1);
		}else {//���Ȃ�
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
		//cout << movables[j] <<"�̏������F"<< node_list[j].getWins() << "   ucb1�F" << ucb1_temp<<"   �g�[�^���g���C�F" << node_list[j].getTotal() << "   �g�[�^���g���C�F" << node_list[j].getTotal() << endl;
		
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
	/*���Ԍv���p
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
			cout << "����" << board.countDisc(BLACK) << " ";
			cout << "����" << board.countDisc(WHITE) << " ";
			cout << "��}�X" << board.countDisc(EMPTY) << endl;
			//cout << board.getCurrentColor() == BLACK ? "����� " : "����� ";
			//���@����擾
			vector<Point> movables = board.getMovablePos();
			//���@���\��
			cout << "���� " << "";
			for (unsigned i = 0; i < movables.size(); i++)
			{
				cout << movables[i] << " ";
			}
			cout << endl << endl;

			Point p;
			string in;
			if (board.getTurns() % 2 == 0) {
				/*�v���C���[�̎��*/
				cout << "���̔Ԃł� " << endl;
				cout << "�����͂��ĉ�����: ";
				cin >> in;
				if (movables.size() == 0)
				{
					in = "p";
					cout << "�u���ꏊ�������̂Ńp�X���܂��B";
				}
				cout << "���́F" << in << "�ɑł��܂����I" << endl;
				if (in == "p")
				{
					// �p�X
					if (!board.pass()) {
						//cerr << board.getTurns() << endl;
						cerr << "�p�X�ł��܂���I" << endl;

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
					cerr << "���o�[�V�`���̎����͂��ĉ������I" << endl;
					continue;
				}
				if (board.move(p) == false)
				{
					cerr << "�����ɂ͒u���܂���I" << endl;
					continue;
				}
			}else{
				cout << "���̔Ԃł� " << endl;
				if (movables.size() >= 2)
				{
					/*���n�����e�J�����@�Ō�����擾*/
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
				cout << "���́F" << in << "�ɑł��܂����I" << endl;
				if (in == "p")
				{
					// �p�X
					if (!board.pass()) {
						//cerr << board.getTurns() << endl;
						cerr << "�p�X�ł��܂���I" << endl;

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
					cerr << "���o�[�V�`���̎����͂��ĉ������I" << endl;
					continue;
				}

				if (board.move(p) == false)
				{
					cerr << "�����ɂ͒u���܂���I" << endl;
					continue;
				}
			}
			if (board.isGameOver())
			{

				cout << "----------------�Q�[���I��----------------" << endl;
				cout << "����" << board.countDisc(BLACK) << " ";
				cout << "����" << board.countDisc(WHITE) << " ";
				if (board.countDisc(BLACK) == board.countDisc(WHITE))
				{
					cout << "���������I" << endl;
				}
				else if (board.countDisc(BLACK) > board.countDisc(WHITE))
				{
					cout << "���̏����ł��I" << endl;
				}
				else {
					cout << "���̏����ł��I" << endl;
				}
				break;
			}
		}

	cout << "���΁F" << white << endl;
	cout << "���΁F" << black << endl;

	printf("�I���\n");
	/*���Ԍv���p
	QueryPerformanceCounter(&end);
	double time = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
	printf("time %lf[ms]\n", time);
	*/
	return 0;
}