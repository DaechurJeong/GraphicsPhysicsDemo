#pragma once

#include "Vector3.h"

class SoftBodyPhysics {
public:
	SoftBodyPhysics() { pos = 0; rot = 0; scale = 1.f; prev_pos = 0; velocity = 0; mass = 0; }
	SoftBodyPhysics(float _m) { pos = 0; rot = 0; scale = 1.f; mass = _m; prev_pos = 0; velocity = 0; }
private:
	vector3 pos;
	vector3 rot;
	vector3 scale;
	float mass;
	vector3 prev_pos;
	vector3 velocity;

};