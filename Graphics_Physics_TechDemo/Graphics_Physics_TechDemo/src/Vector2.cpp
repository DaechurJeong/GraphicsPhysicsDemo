#include "Vector2.h"
#include <cmath>


void vector2::operator=(const vector2& _rhs)
{
	x = _rhs.x;
	y = _rhs.y;
}

void vector2::operator+=(const vector2& _rhs)
{
	x += _rhs.x;
	y += _rhs.y;
}

void vector2::operator-=(const vector2& _rhs)
{
	x -= _rhs.x;
	y -= _rhs.y;
}

void vector2::operator+=(const float& val)
{
	x += val;
	y += val;
}

void vector2::operator-=(const float& val)
{
	x -= val;
	y -= val;
}

void vector2::operator*=(const float& val)
{
	x *= val;
	y *= val;
}

void vector2::operator/=(const float& val)
{
	x /= val;
	y /= val;
}

bool vector2::operator==(const vector2& _rhs)
{
	return ((x == _rhs.x && y == _rhs.y) ? true : false);
}

float vector2::getDistance()
{
	return sqrtf(x * x + y * y);
}

float vector2::getSquaredDistance()
{
	return x * x + y * y;
}

vector2 vector2::operator+(const vector2& _rhs)
{
	vector2 result = *this;
	result += _rhs;
	return result;
}

vector2 vector2::operator-(const vector2& _rhs)
{
	vector2 result = *this;
	result -= _rhs;
	return result;
}

vector2 vector2::operator+(const float& _rhs)
{
	vector2 result = *this;
	result.x += _rhs;
	result.y += _rhs;
	return result;
}

vector2 vector2::operator-(const float& _rhs)
{
	vector2 result = *this;
	result.x -= _rhs;
	result.y -= _rhs;
	return result;
}

vector2 vector2::operator-()
{
	return vector2(-x, -y);
}


vector2 vector2::dot(const vector2& _rhs)
{
	vector2 result = *this;
	result.x *= _rhs.x;
	result.y *= _rhs.y;
	return result;
}
