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

template<typename T> class Vec3D
	{
public:
	T x;
	T y;
	T z;

	Vec3D(T x, T y, T z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3D(const Vec3D<int> &s)
	{
		this->x = (T)s.x;
		this->y = (T)s.y;
		this->z = (T)s.z;
	}

	Vec3D(const Vec3D<float> &s)
	{
		this->x = (T)s.x;
		this->y = (T)s.y;
		this->z = (T)s.z;
	}

	Vec3D()
	{
		x = y = z = 0;
	}

	float length()
	{
		float dx = (float)x;
		float dy = (float)y;
		float dz = (float)z;
		return sqrtf(dx*dx + dy*dy + dz*dz);
	}

	T volume()
	{
		return x * y * z;
	}

	void operator=(const Vec3D<T> &from)
	{
		x = from.x;
		y = from.y;
		z = from.z;
	}

	inline Vec3D<T> operator+(const T &rhs)
	{
		Vec3D<T> s;
		s.x = this->x + rhs;
		s.y = this->y + rhs;
		s.z = this->z + rhs;
		return s;
	}

	inline Vec3D<T> operator-(const T &rhs)
	{
		Vec3D<T> s;
		s.x = this->x - rhs;
		s.y = this->y - rhs;
		s.z = this->z - rhs;
		return s;
	}

	inline Vec3D<T> operator*(const T &rhs)
	{
		Vec3D<T> s;
		s.x = this->x * rhs;
		s.y = this->y * rhs;
		s.z = this->z * rhs;
		return s;
	}

	inline Vec3D<T> operator/(const T &rhs)
	{
		Vec3D<T> s;
		s.x = this->x / rhs;
		s.y = this->y / rhs;
		s.z = this->z / rhs;
		return s;
	}

	inline Vec3D<T> &operator+=(const T &rhs)
	{
		this->x += rhs;
		this->y += rhs;
		this->z += rhs;
		return *this;
	}

	inline Vec3D<T> &operator-=(const T &rhs)
	{
		this->x -= rhs;
		this->y -= rhs;
		this->z -= rhs;
		return *this;
	}

	inline Vec3D<T> &operator*=(const T &rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}

	inline Vec3D<T> &operator/=(const T &rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;
		return *this;
	}

	inline Vec3D<T> &operator-=(const Vec3D<T> &rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	inline Vec3D<T> &operator+=(const Vec3D<T> &rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	inline Vec3D<T> &operator/=(const Vec3D<T> &rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
		return *this;
	}

	inline Vec3D<T> &operator*=(const Vec3D<T> &rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
		return *this;
	}

	inline Vec3D<T> operator-(const Vec3D<T> &rhs)
	{
		Vec3D<T> s(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
		return s;
	}

	inline Vec3D<T> operator+(const Vec3D<T> &rhs)
	{
		Vec3D<T> s(this->x + rhs.x, this->x + rhs.y, this->z + rhs.z);
		return s;
	}

	inline Vec3D<T> operator/(const Vec3D<T> &rhs)
	{
		Vec3D<T> s(this->x / rhs.x, this->x / rhs.y, this->z / rhs.z);
		return s;
	}

	inline Vec3D<T> operator*(const Vec3D<T> &rhs)
	{
		Vec3D<T> s(this->x * rhs.x, this->x * rhs.y, this->z * rhs.z);
		return s;
	}

	inline Vec3D<T> operator-()
	{
		Vec3D<T> s;
		s.x = -this->x;
		s.y = -this->y;
		s.z = -this->z;
		return s;
	}

	inline T dot(Vec3D<T> v2) {
		return x * v2.x + y * v2.y + z * v2.z;
	}

	inline Vec3D<T> cross(Vec3D<T> v2) {
		Vec3D<T> v;
		v.x = y * v2.z - v2.y * z;
		v.y = z * v2.x - v2.z * x;
		v.z = x * v2.y - v2.x * y;
		return v;
	}
};

} // End namespace Nooskewl_Engine

#endif // TYPES_H
