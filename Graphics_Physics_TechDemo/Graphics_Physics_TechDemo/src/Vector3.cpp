#include "Vector3.h"
#include <cmath>

float vector3::getDistance()
{
	return sqrtf(x * x + y * y + z * z);
}

float vector3::getSquaredDistance()
{
	return x * x + y * y + z * z;
}

vector3 vector3::dot(const vector3& _rhs)
{
	vector3 result = *this;
	result.x *= _rhs.x;
	result.y *= _rhs.y;
	result.z *= _rhs.z;
	return result;
}

vector3 vector3::operator+(const vector3& _rhs)
{
	vector3 result = *this;
	result += _rhs;
	return result;
}

vector3 vector3::operator-(const vector3& _rhs)
{
	vector3 result = *this;
	result -= _rhs;
	return result;
}

vector3 vector3::operator+(const float& _rhs)
{
	vector3 result = *this;
	result.x += _rhs;
	result.y += _rhs;
	result.z += _rhs;
	return result;
}

vector3 vector3::operator-(const float& _rhs)
{
	vector3 result = *this;
	result.x -= _rhs;
	result.y -= _rhs;
	result.z -= _rhs;
	return result;
}

vector3 vector3::operator*(const float& _rhs)
{
	vector3 result = *this;
	result.x *= _rhs;
	result.y *= _rhs;
	result.z *= _rhs;
	return result;
}

vector3 vector3::operator-()
{
	return vector3(-x, -y, -z);
}

void vector3::operator=(const vector3& _rhs)
{
	x = _rhs.x;
	y = _rhs.y;
	z = _rhs.z;
}

void vector3::operator=(const float& _rhs)
{
	x = _rhs;
	y = _rhs;
	z = _rhs;
}

void vector3::operator+=(const vector3& _rhs)
{
	x += _rhs.x;
	y += _rhs.y;
	z += _rhs.z;
}

void vector3::operator-=(const vector3& _rhs)
{
	x -= _rhs.x;
	y -= _rhs.y;
	z -= _rhs.z;
}

void vector3::operator+=(const float& val)
{
	x += val;
	y += val;
	z += val;
}

void vector3::operator-=(const float& val)
{
	x -= val;
	y -= val;
	z -= val;
}

void vector3::operator*=(const float& val)
{
	x *= val;
	y *= val;
	z *= val;
}

void vector3::operator/=(const float& val)
{
	x /= val;
	y /= val;
	z /= val;
}

bool vector3::operator==(const vector3& _rhs)
{
	return ((x == _rhs.x && y == _rhs.y && z == _rhs.z) ? true : false);
}
