#include "Scene.h"

void Scene::GenerateObjectsSceneOne(Physics& physics, std::vector<Object>& main_objs, std::vector<Object>& pbr_objs, std::vector<SoftBodyPhysics>& physics_objs)
{
	unsigned num_obj = 6;
	for (unsigned i = 0; i < num_obj; ++i)
	{
		Object objs(O_SPHERE, glm::vec3(-5.f + 3 * i, 0.f, -2.f), glm::vec3(1.f, 1.f, 1.f), 64);
		main_objs.push_back(objs);
		scene_obj.push_back(objs);
	}
	Object main_obj_texture(O_SPHERE, glm::vec3(0.9f, -2.5f, 2.f), glm::vec3(1.f, 1.f, 1.f), 64);
	physics.push_object(&main_obj_texture);
	pbr_objs.push_back(main_obj_texture);
	scene_obj.push_back(main_obj_texture);

	SoftBodyPhysics plane(O_PLANE, glm::vec3(0, 1.5f, 1.f), glm::vec3(4.f, 1.f, 7.f), 64);
	physics.push_object(&plane);
	physics_objs.push_back(plane);
	scene_obj.push_back(plane);
}