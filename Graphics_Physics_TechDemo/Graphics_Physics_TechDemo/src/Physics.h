#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

//#include "Base.h"
//#include "Object.h"
#include "glm/glm.hpp"
#include <vector>


#define MIN_LEAF_SIZE 300

class Object;
class SoftBodyPhysics;




class Physics {
public:
	//Physics(Object& obj) { physics_objs.push_back(obj); }
	void update(float dt);
	void push_object(Object* _obj) { physics_objs.push_back(_obj);}
	void push_object(SoftBodyPhysics* _obj) { softbody_objs.push_back(_obj); }

	void delete_object(Object* obj);
	bool empty() { return physics_objs.empty(); }
	void clear_objects() {
		//for (auto ph : physics_objs)
		//{
		//	delete ph;
		//	ph = NULL;
		//}
		physics_objs.clear();
		//for (auto sf : softbody_objs)
		//{
		//	delete sf;
		//	sf = NULL;
		//}
		softbody_objs.clear();
	};


private:

	std::vector<Object*> physics_objs;
	std::vector<SoftBodyPhysics*> softbody_objs;

};

#endif // !PHYSICS_H