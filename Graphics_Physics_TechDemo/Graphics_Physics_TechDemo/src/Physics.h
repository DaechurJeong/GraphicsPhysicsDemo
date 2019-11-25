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
class Camera;
class Shader;

enum TreeType {
	NODE,
	LEAF
};

struct OcNode {
	glm::vec3 center;
	float halfwidth;
	OcNode* pChild[8];
	std::vector<std::vector<glm::vec3>> triangles;
	std::vector<std::pair<unsigned, std::vector<unsigned>>> indices;
	int depth;
	TreeType type;
	
	OcNode(int _depth, std::vector<std::pair<unsigned, std::vector<unsigned>>> _index, std::vector<std::vector<glm::vec3>> _triangle)
	{
		depth = _depth;
		triangles = _triangle;
		indices = _index;
	}

	void Draw(Camera* camera, Shader* shader, Object* line);
};

class Physics {
public:
	//Physics(Object& obj) { physics_objs.push_back(obj); }
	void update(float dt);
	void push_object(Object* _obj) { physics_objs.push_back(_obj);}
	void push_object(SoftBodyPhysics* _obj) { softbody_objs.push_back(_obj); }

	OcNode* buildOctree(glm::vec3 center, float halfwidth, int depth, std::vector<std::vector<glm::vec3>>& triangles,
		std::vector<std::pair<unsigned, std::vector<unsigned>>>& indices, std::vector<SoftBodyPhysics*>& objs);

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

	OcNode* OcTree;
private:
	bool isInside(std::vector<glm::vec3> polygon, glm::vec3 center, float half);




	std::vector<Object*> physics_objs;
	std::vector<SoftBodyPhysics*> softbody_objs;

};

#endif // !PHYSICS_H