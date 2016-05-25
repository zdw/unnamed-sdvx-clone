#pragma once
#include "Rect.hpp"
#include "Math.hpp"
#include <limits>

template<typename T>
class MarginBase
{
public:
	MarginBase() 
		: left(0), right(0), top(0), bottom(0) {};
	MarginBase(T x, T y) 
		: left(x), right(x), top(y), bottom(y) {};
	MarginBase(T left, T top, T right, T bottom) 
		: left(left), top(top), right(right), bottom(bottom) {};

	RectangleBase<T> Apply(const RectangleBase<T>& in) const
	{
		RectangleBase<T> ret = in;
		ret.pos += VectorBase<T, 2>(left, top);
		ret.size -= VectorBase<T, 2>(left + right, top + bottom);
		return ret;
	}

	MarginBase operator+(const MarginBase& other) const
	{
		return MarginBase(
			left + other.left, 
			top + other.top,
			right + other.right, 
			bottom + other.bottom);
	}
	MarginBase& operator+=(const MarginBase& other)
	{
		return *this = (*this) + other;
	}
	MarginBase operator-() const
	{
		return MarginBase(
			-left,
			-top,
			-right,
			-bottom);
	}

	T Width() const
	{
		return left + right;
	}
	T Height() const
	{
		return top + bottom;
	}

	void ClampMin(T min = 0)
	{
		left = Math::Max(left, min);
		top = Math::Max(top, min);
		right = Math::Max(right, min);
		bottom = Math::Max(bottom, min);
	}
	void Clamp(T min, T max)
	{
		left = Math::Clamp(left, min, max);
		top = Math::Clamp(top, min, max);
		right = Math::Clamp(right, min, max);
		bottom = Math::Clamp(bottom, min, max);
	}

	T left, top, right, bottom;
};

typedef MarginBase<float> Margin;
typedef MarginBase<int32> Margini;