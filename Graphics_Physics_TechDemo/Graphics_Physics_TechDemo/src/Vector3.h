#pragma once
#include "Vector2.h"

struct vector3 {
	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }
	void setX(float _x) { x = _x; }
	void setY(float _y) { y = _y; }
	void setZ(float _z) { z = _z; }
	float getDistance();
	float getSquaredDistance();
	vector3 dot(const vector3& _rhs);

	//constructor
	vector3() = default;
	vector3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
	vector3(const vector3& _rhs) { *this = _rhs; }

	//operator

	vector3 operator+(const vector3& _rhs);
	vector3 operator-(const vector3& _rhs);
	vector3 operator+(const float& _rhs);
	vector3 operator-(const float& _rhs);
	vector3 operator-();
	void operator=(const vector3& _rhs);
	void operator=(const float& _rhs);

	void operator+=(const vector3& _rhs);
	void operator-=(const vector3& _rhs);
	void operator+=(const float& val);
	void operator-=(const float& val);
	void operator*=(const float& val);
	void operator/=(const float& val);
	bool operator==(const vector3& _rhs);



	//data
	float x;
	float y;
	float z;
};