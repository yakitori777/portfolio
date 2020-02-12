#ifndef REVERSI_H_INCLUDED
#define REVERSI_H_INCLUDED

#include <string>
#include <sstream>
#include <stdexcept>

const int BOARD_SIZE =  8;
const int MAX_TURNS  = 60;


typedef int Color;

const Color EMPTY = 0;
const Color WHITE = -1;
const Color BLACK = 1;
const Color WALL  = 2;



struct Point
{

	int x;
	int y;
	
	Point()
	{
		Point(0, 0);
	}
	
	Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	
	Point(std::string coordstr) throw(std::invalid_argument)
	{
		if(coordstr.length() < 2)
			throw std::invalid_argument("The argument must be Reversi style coordinates!");
		
		x = coordstr[0] - 'a'+1;
		y = coordstr[1] - '1'+1;
	}
	
	operator std::string() const
	{
		std::ostringstream oss;
		oss << static_cast<char>('a'+x-1) << static_cast<char>('1'+y-1);
		
		return oss.str();
	}

	bool operator==(const Point& p) const
	{
		if(x != p.x) return false;
		if(y != p.y) return false;
		
		return true;
	}
};
/*
bool operator==(const Point& lhs, const Point& rhs)
{
	if(lhs.x != rhs.x) return false;
	if(lhs.y != rhs.y) return false;
	
	return true;
}*/

struct Disc : public Point
{
	Color color;

	Disc() : Point(0, 0)
	{
		color = EMPTY;
	}

	Disc(int x, int y, Color color) : Point(x, y)
	{
		this->color = color;
	}
};

template<typename T> class ColorStorage
{
	T data[3];
public:
	T& operator[](int color)
	{
		return data[color+1];
	}

	const T& operator[](int color) const
	{
		return data[color+1];
	}

	ColorStorage<T>& operator +=(const ColorStorage<T> &src)
	{
		data[0] += src.data[0];
		data[1] += src.data[1];
		data[2] += src.data[2];

		return *this;
	}

};

#endif
