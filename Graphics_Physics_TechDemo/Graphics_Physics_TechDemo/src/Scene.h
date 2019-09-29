#pragma once

#ifndef SCENE_H
#define SCENE_H

#include "Object.h"
#include "Base.h"
#include "Physics.h"

class Scene {
public:
	Scene(int sceneNum);
	~Scene();
	void GenerateObjectsSceneOne(Physics& physics, std::vector<Object>& main_objs, std::vector<Object>& pbr_objs, std::vector<SoftBodyPhysics>& physics_objs);
	void Update();
	void Draw();

private:
	unsigned curr_scene;
	std::vector<Object&> scene_obj;
	Physics m_physics;
};


#endif