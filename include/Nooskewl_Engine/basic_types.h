#ifndef TYPES_H
#define TYPES_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

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

	Size(const Size<int> &s)
	{
		this->w = (T)s.w;
		this->h = (T)s.h;
	}

	Size(const Size<float> &s)
	{
		this->w = (T)s.w;
		this->h = (T)s.h;
	}

	Size() {}

	float length()
	{
		float dx = (float)w;
		float dy = (float)h;
		return sqrtf(dx*dx + dy*dy);
	}

	float angle()
	{
		float dx = (float)w;
		float dy = (float)h;
		return atan2f(dy, dx);
	}

	T area()
	{
		return w * h;
	}

	void operator=(const Size<T> &from)
	{
		w = from.w;
		h = from.h;
	}

	inline Size<T> operator+(const T &rhs)
	{
		Size<T> s;
		s.w = this->w + rhs;
		s.h = this->h + rhs;
		return s;
	}

	inline Size<T> operator-(const T &rhs)
	{
		Size<T> s;
		s.w = this->w - rhs;
		s.h = this->h - rhs;
		return s;
	}

	inline Size<T> operator*(const T &rhs)
	{
		Size<T> s;
		s.w = this->w * rhs;
		s.h = this->h * rhs;
		return s;
	}

	inline Size<T> operator/(const T &rhs)
	{
		Size<T> s;
		s.w = this->w / rhs;
		s.h = this->h / rhs;
		return s;
	}

	inline Size<T> &operator+=(const T &rhs)
	{
		this->w += rhs;
		this->h += rhs;
		return *this;
	}

	inline Size<T> &operator-=(const T &rhs)
	{
		this->w -= rhs;
		this->h -= rhs;
		return *this;
	}

	inline Size<T> &operator*=(const T &rhs)
	{
		this->w *= rhs;
		this->h *= rhs;
		return *this;
	}

	inline Size<T> &operator/=(const T &rhs)
	{
		this->w /= rhs;
		this->h /= rhs;
		return *this;
	}

	inline Size<T> &operator-=(const Size<T> &rhs)
	{
		this->w -= rhs.w;
		this->h -= rhs.h;
		return *this;
	}

	inline Size<T> &operator+=(const Size<T> &rhs)
	{
		this->w += rhs.w;
		this->h += rhs.h;
		return *this;
	}

	inline Size<T> &operator/=(const Size<T> &rhs)
	{
		this->w /= rhs.w;
		this->h /= rhs.h;
		return *this;
	}

	inline Size<T> &operator*=(const Size<T> &rhs)
	{
		this->w *= rhs.w;
		this->h *= rhs.h;
		return *this;
	}

	inline Size<T> operator-(const Size<T> &rhs)
	{
		Size<T> s(this->w - rhs.w, this->h - rhs.h);
		return s;
	}

	inline Size<T> operator+(const Size<T> &rhs)
	{
		Size<T> s(this->w + rhs.w, this->w + rhs.h);
		return s;
	}

	inline Size<T> operator/(const Size<T> &rhs)
	{
		Size<T> s(this->w / rhs.w, this->w / rhs.h);
		return s;
	}

	inline Size<T> operator*(const Size<T> &rhs)
	{
		Size<T> s(this->w * rhs.w, this->w * rhs.h);
		return s;
	}

	inline Size<T> operator-()
	{
		Size<T> s;
		s.w = -this->w;
		s.h = -this->h;
		return s;
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

	Point(const Point<int> &p)
	{
		this->x = (T)p.x;
		this->y = (T)p.y;
	}

	Point(const Point<float> &p)
	{
		this->x = (T)p.x;
		this->y = (T)p.y;
	}

	float length()
	{
		float dx = (float)x;
		float dy = (float)y;
		return sqrtf(dx*dx + dy*dy);
	}

	float angle()
	{
		float dx = (float)x;
		float dy = (float)y;
		return atan2f(dy, dx);
	}

	inline void operator=(const Point<T> &from)
	{
		x = from.x;
		y = from.y;
	}

	inline bool operator==(const Point<T> &rhs)
	{
		return this->x == rhs.x && this->y == rhs.y;
	}

	inline bool operator!=(const Point<T> &rhs)
	{
		return this->x != rhs.x || this->y != rhs.y;
	}

	inline Point<T> operator+(const T &rhs)
	{
		Point<T> p;
		p.x = this->x + rhs;
		p.y = this->y + rhs;
		return p;
	}

	inline Point<T> operator-(const T &rhs)
	{
		Point<T> p;
		p.x = this->x - rhs;
		p.y = this->y - rhs;
		return p;
	}

	inline Point<T> operator*(const T &rhs)
	{
		Point<T> p;
		p.x = this->x * rhs;
		p.y = this->y * rhs;
		return p;
	}

	inline Point<T> operator/(const T &rhs)
	{
		Point<T> p;
		p.x = this->x / rhs;
		p.y = this->y / rhs;
		return p;
	}

	inline Point<T> &operator+=(const T &rhs)
	{
		this->x += rhs;
		this->y += rhs;
		return *this;
	}

	inline Point<T> &operator-=(const T &rhs)
	{
		this->x -= rhs;
		this->y -= rhs;
		return *this;
	}

	inline Point<T> &operator*=(const T &rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}

	inline Point<T> &operator/=(const T &rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}

	inline Point<T> operator+(const Point<T> &rhs)
	{
		Point<T> p;
		p.x = this->x + rhs.x;
		p.y = this->y + rhs.y;
		return p;
	}

	inline Point<T> operator-(const Point<T> &rhs)
	{
		Point<T> p;
		p.x = this->x - rhs.x;
		p.y = this->y - rhs.y;
		return p;
	}

	inline Point<T> operator*(const Point<T> &rhs)
	{
		Point<T> p;
		p.x = this->x * rhs.x;
		p.y = this->y * rhs.y;
		return p;
	}

	inline Point<T> operator/(const Point<T> &rhs)
	{
		Point<T> p;
		p.x = this->x / rhs.x;
		p.y = this->y / rhs.y;
		return p;
	}

	inline Point<T> &operator+=(const Point<T> &rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	inline Point<T> &operator-=(const Point<T> &rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	inline Point<T> &operator*=(const Point<T> &rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		return *this;
	}

	inline Point<T> &operator/=(const Point<T> &rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		return *this;
	}

	inline Point<T> operator+(const Size<T> &rhs)
	{
		Point<T> p;
		p.x = this->x + rhs.w;
		p.y = this->y + rhs.h;
		return p;
	}

	inline Point<T> operator-(const Size<T> &rhs)
	{
		Point<T> p;
		p.x = this->x - rhs.w;
		p.y = this->y - rhs.h;
		return p;
	}

	inline Point<T> operator*(const Size<T> &rhs)
	{
		Point<T> p;
		p.x = this->x * rhs.w;
		p.y = this->y * rhs.h;
		return p;
	}

	inline Point<T> operator/(const Size<T> &rhs)
	{
		Point<T> p;
		p.x = this->x / rhs.w;
		p.y = this->y / rhs.h;
		return p;
	}

	inline Point<T> &operator+=(const Size<T> &rhs)
	{
		this->x += rhs.w;
		this->y += rhs.h;
		return *this;
	}

	inline Point<T> &operator-=(const Size<T> &rhs)
	{
		this->x -= rhs.w;
		this->y -= rhs.h;
		return *this;
	}

	inline Point<T> &operator*=(const Size<T> &rhs)
	{
		this->x *= rhs.w;
		this->y *= rhs.h;
		return *this;
	}

	inline Point<T> &operator/=(const Size<T> &rhs)
	{
		this->x /= rhs.w;
		this->y /= rhs.h;
		return *this;
	}

	inline Point<T> operator-()
	{
		Point<T> p;
		p.x = -this->x;
		p.y = -this->y;
		return p;
	}
};

} // End namespace Nooskewl_Engine

#endif // TYPES_H
