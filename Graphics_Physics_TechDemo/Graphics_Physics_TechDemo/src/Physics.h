/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Physics.h
Purpose: Prototype of Physics class
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Nahye Park, nahye.park
Creation date: 9/20/2018
End Header --------------------------------------------------------*/
#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

#include "glm/glm.hpp"
#include <vector>

#define MIN_LEAF_SIZE 300

class Object;
class SoftBodyPhysics;

class Physics {
public:
	void update(float dt);
	void push_object(Object* _obj) { physics_objs.push_back(_obj);}
	void push_object(SoftBodyPhysics* _obj) { softbody_objs.push_back(_obj); }

	void delete_object(Object* obj);
	bool empty() { return physics_objs.empty(); }
	void clear_objects() {
		physics_objs.clear();
		softbody_objs.clear();
	};
private:
	std::vector<Object*> physics_objs;
	std::vector<SoftBodyPhysics*> softbody_objs;

};

#endif // !PHYSICS_H