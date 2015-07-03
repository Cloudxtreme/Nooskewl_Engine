#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <cmath>

template<typename T> class Size
	{
public:
	T w;
	T h;

	Size(T w, T h)
	{
		this->w = w;
		this->h = h;
	}

	Size() {}

	void operator=(const Size<T> &from)
	{
		w = from.w;
		h = from.h;
	}
};

template<typename T> class Point
{
public:
	T x;
	T y;

	Point()
	{
		x = y = 0;
	}

	Point(T x, T y)
	{
		this->x = x;
		this->y = y;
	}

	Point(const Point<float> &p)
	{
		this->x = p.x;
		this->y = p.y;
	}

	Point(const Point<int> &p)
	{
		this->x = p.x;
		this->y = p.y;
	}

	inline void operator=(const Point<float> &from)
	{
		x = from.x;
		y = from.y;
	}

	inline void operator=(const Point<int> &from)
	{
		x = from.x;
		y = from.y;
	}

	inline bool operator==(const Point<T> &rhs)
	{
		return this->x == rhs.x && this->y == rhs.y;
	}

	inline Point<T> operator+(const Point<float> &rhs)
	{
		Point<T> p;
		p.x = this->x + rhs.x;
		p.y = this->y + rhs.y;
		return p;
	}

	inline Point<T> operator-(const Point<float> &rhs)
	{
		Point<T> p;
		p.x = this->x - rhs.x;
		p.y = this->y - rhs.y;
		return p;
	}

	inline Point<T> operator+(const Point<int> &rhs)
	{
		Point<T> p;
		p.x = this->x + rhs.x;
		p.y = this->y + rhs.y;
		return p;
	}

	inline Point<T> operator-(const Point<int> &rhs)
	{
		Point<T> p;
		p.x = this->x - rhs.x;
		p.y = this->y - rhs.y;
		return p;
	}

	inline Point<T> operator+(const Size<float> &rhs)
	{
		Point<T> p;
		p.x = this->x + rhs.w;
		p.y = this->y + rhs.h;
		return p;
	}

	inline Point<T> operator-(const Size<float> &rhs)
	{
		Point<T> p;
		p.x = this->x - rhs.w;
		p.y = this->y - rhs.h;
		return p;
	}

	inline Point<T> operator+(const Size<int> &rhs)
	{
		Point<T> p;
		p.x = this->x + rhs.w;
		p.y = this->y + rhs.h;
		return p;
	}

	inline Point<T> operator-(const Size<int> &rhs)
	{
		Point<T> p;
		p.x = this->x - rhs.w;
		p.y = this->y - rhs.h;
		return p;
	}

	inline const Point<T> &operator/(const float &rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}

	inline Point<T> &operator+=(const Point<T> &b)
	{
		this->x += b.x;
		this->y += b.y;
		return *this;
	}

	inline Point<T> &operator-=(const Point<T> &b)
	{
		this->x -= b.x;
		this->y -= b.y;
		return *this;
	}

	inline Point<T> &operator+=(const T &b)
	{
		this->x += b;
		this->y += b;
		return *this;
	}

	inline Point<T> &operator-=(const T &b)
	{
		this->x -= b;
		this->y -= b;
		return *this;
	}
};

template<typename T> class Four_Corners
{
public:
	T x1, y1, x2, y2;

	Four_Corners(T x1, T y1, T x2, T y2)
	{
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
	}

	Four_Corners() {}

	void operator=(const Four_Corners<T> &rhs)
	{
		x1 = rhs.x1;
		y1 = rhs.y1;
		x2 = rhs.x2;
		y2 = rhs.y2;
	}
};

#define Line Four_Corners
#define Rectangle Four_Corners

#endif // TYPES_H