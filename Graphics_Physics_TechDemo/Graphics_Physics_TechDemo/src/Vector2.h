#pragma once


struct vector2 {
	float getX() { return x; }
	float getY() { return y; }
	void setX(float _x) { x = _x; }
	void setY(float _y) { y = _y; }
	float getDistance();
	float getSquaredDistance();
	vector2 dot(const vector2& _rhs);

	//constructor
	vector2() = default;
	vector2(float _x, float _y) { x = _x; y = _y; }
	vector2(const vector2& _rhs) { *this = _rhs; }

	//operator
	
	vector2 operator+(const vector2& _rhs);
	vector2 operator-(const vector2& _rhs);
	vector2 operator+(const float& _rhs);
	vector2 operator-(const float& _rhs);
	vector2 operator-();
	void operator=(const vector2& _rhs);
	void operator+=(const vector2& _rhs);
	void operator-=(const vector2& _rhs);
	void operator+=(const float& val);
	void operator-=(const float& val);
	void operator*=(const float& val);
	void operator/=(const float& val);
	bool operator==(const vector2& _rhs);


	//data
	float x;
	float y;
};