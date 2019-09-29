#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

//#include "Base.h"
//#include "Object.h"

#include <vector>

class Object;
class SoftBodyPhysics;

class Physics {
public:
	//Physics(Object& obj) { physics_objs.push_back(obj); }
	void update(float dt);
	void push_object(Object* _obj) { physics_objs.push_back(_obj);}
	void push_object(SoftBodyPhysics* _obj) { softbody_objs.push_back(_obj); }

	void delete_object(Object* obj);
private:
	
	std::vector<Object*> physics_objs;
	std::vector<SoftBodyPhysics*> softbody_objs;

};

#endif // !PHYSICS_H