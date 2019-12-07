/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene.cpp
Purpose: Scene manager to control 6 scenes and UI
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Charlie Jung, jungdae.chur
Creation date: 10/6/2018
End Header --------------------------------------------------------*/
#include "Scene.h"
#include "input.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

const float FRAME_LIMIT = 1.f / 59.f;
const float PI = 4.0f * atan(1.0f);

void Scene::Init(GLFWwindow* window, Camera* camera)
{
	pbr_texture_shader.CreateShader("ShaderCodes\\pbr_texture.vs", "ShaderCodes\\pbr_texture.fs", nullptr);
	equirectangularToCubmapShader.CreateShader("ShaderCodes\\cubemap.vs", "ShaderCodes\\equirectangular_to_cubemap.fs", nullptr);
	irradianceShader.CreateShader("ShaderCodes\\cubemap.vs", "ShaderCodes\\irradiance_convolution.fs", nullptr);
	backgroundShader.CreateShader("ShaderCodes\\background.vs", "ShaderCodes\\background.fs", nullptr);
	brdfShader.CreateShader("ShaderCodes\\brdf.vs", "ShaderCodes\\brdf.fs", nullptr);
	prefilterShader.CreateShader("ShaderCodes\\cubemap.vs", "ShaderCodes\\prefilter.fs", nullptr);
	lightShader.CreateShader("ShaderCodes\\pbr_texture.vs", "ShaderCodes\\light.fs", nullptr);

	pbr_texture_shader.Use();
	pbr_texture_shader.SetInt("irradianceMap", 0);
	pbr_texture_shader.SetInt("prefilterMap", 1);
	pbr_texture_shader.SetInt("brdfLUT", 2);

	pbr_texture_shader.SetInt("albedo", 3);
	pbr_texture_shader.SetInt("normal", 4);
	pbr_texture_shader.SetInt("metallic", 5);
	pbr_texture_shader.SetInt("roughness", 6);
	pbr_texture_shader.SetInt("ao", 7);

	backgroundShader.Use();
	backgroundShader.SetInt("environmentMap", 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// pbr: setup framebuffer
	InitFrameBuffer(&equirectangularToCubmapShader, &irradianceShader, &prefilterShader, &brdfShader,
		captureFBO, captureRBO, envCubemap, irradianceMap, prefilterMap, brdfLUTTexture, hdrTexture);
	InitSkybox(&backgroundShader, &pbr_texture_shader, camera, (float)width, (float)height);

	ResizeFrameBuffer(window);

	if (curr_scene == 0)
		Scene0Init(camera);
	else if (curr_scene == 1)
		Scene1Init(camera);
	else if (curr_scene == 2)
		Scene2Init(camera);
	else if (curr_scene == 3)
		Scene3Init(camera);
	else if (curr_scene == 4)
		Scene4Init(camera);
	else if (curr_scene == 5)
		Scene5Init(camera);
}
void Scene::Update(GLFWwindow* window, Camera* camera, float dt)
{
	float currFrame = (float)glfwGetTime();
	deltaTime = currFrame - lastFrame;
	lastFrame = currFrame;

	ImGuiUpdate(window, camera, deltaTime);

	ProcessInput(camera, window, deltaTime);

	if (curr_scene == 0)
		Scene0Draw(window, camera, deltaTime);
	else if (curr_scene == 1)
		Scene1Draw(camera, deltaTime);
	else if (curr_scene == 2)
		Scene2Draw(camera, deltaTime);
	else if (curr_scene == 3)
		Scene3Draw(camera, deltaTime);
	else if (curr_scene == 4)
		Scene4Draw(camera, deltaTime);
	else if (curr_scene == 5)
		Scene5Draw(camera, deltaTime);

	ImGuirender();
}
void Scene::Scene0Init(Camera* camera)
{
	Object* rigid_plane = new Object(O_PLANE, glm::vec3(4.f, -4.f, 1.f), glm::vec3(7.f, 1.f, 0.5f), P_DIMENSION);
	rigid_plane->rotation = 1.f;
	m_physics.push_object(rigid_plane);
	pbr_obj.push_back(rigid_plane);

	Object* rigid_plane_1 = new Object(O_PLANE, glm::vec3(4.f, -4.f, 2.f), glm::vec3(7.f, 1.f, 0.5f), P_DIMENSION);
	rigid_plane_1->rotation = 1.f;
	m_physics.push_object(rigid_plane_1);
	pbr_obj.push_back(rigid_plane_1);

	Object* rigid_plane_2 = new Object(O_PLANE, glm::vec3(1.3f, -7.f, 0.5f), glm::vec3(4.f, 1.f, 4.f), P_DIMENSION); // 2
	rigid_plane_2->rotation = -1.f;
	m_physics.push_object(rigid_plane_2);
	pbr_obj.push_back(rigid_plane_2);

	Object* rigid_plane_3 = new Object(O_PLANE, glm::vec3(4.f, -10.3f, 0.5f), glm::vec3(4.f, 1.f, 4.f), P_DIMENSION); // 3
	rigid_plane_3->rotation = 1.f;
	m_physics.push_object(rigid_plane_3);
	pbr_obj.push_back(rigid_plane_3);

	Object* rigid_plane_4 = new Object(O_PLANE, glm::vec3(1.3f, -13.f, 0.5f), glm::vec3(4.f, 1.f, 4.f), P_DIMENSION); // 2
	rigid_plane_4->rotation = -1.f;
	m_physics.push_object(rigid_plane_4);
	pbr_obj.push_back(rigid_plane_4);

	Object* rigid_plane_5 = new Object(O_PLANE, glm::vec3(4.f, -16.3f, 0.5f), glm::vec3(4.f, 1.f, 4.f), P_DIMENSION); // 3
	rigid_plane_5->rotation = 1.f;
	m_physics.push_object(rigid_plane_5);
	pbr_obj.push_back(rigid_plane_5);

	Object* rigid_plane_6 = new Object(O_PLANE, glm::vec3(1.3f, -19.f, 0.5f), glm::vec3(4.f, 60.f, 4.f), P_DIMENSION); // 2
	rigid_plane_6->rotation = -0.1f;
	m_physics.push_object(rigid_plane_6);
	pbr_obj.push_back(rigid_plane_6);

	Object* rigid_plane_7 = new Object(O_PLANE, glm::vec3(8.f, -40.3f, 0.5f), glm::vec3(4.f, 40.f, 4.f), P_DIMENSION); // 3
	rigid_plane_7->rotation = 0.1f;
	m_physics.push_object(rigid_plane_7);
	pbr_obj.push_back(rigid_plane_7);

	//right
	Object* rigid_cube_1 = new Object(O_PLANE, glm::vec3(4.5f, -52.f, 0.5f), glm::vec3(2.f, 2.f, 3.0f), P_DIMENSION);
	rigid_cube_1->axis = glm::vec3(0.f, 0.f, 1.f);
	rigid_cube_1->rotation = 1.5708f;
	m_physics.push_object(rigid_cube_1);
	pbr_obj.push_back(rigid_cube_1);

	//left
	Object* rigid_cube_2 = new Object(O_PLANE, glm::vec3(1.5f, -50.f, 0.5f), glm::vec3(2.f, 2.f, 3.0f), P_DIMENSION);
	rigid_cube_2->axis = glm::vec3(0.f, 0.f, 1.f);
	rigid_cube_2->rotation = -1.5708f;
	m_physics.push_object(rigid_cube_2);
	pbr_obj.push_back(rigid_cube_2);

	//back
	Object* rigid_cube_3 = new Object(O_PLANE, glm::vec3(1.5f, -50.f, 0.5f), glm::vec3(3.0f, 2.f, 2.f), P_DIMENSION);
	rigid_cube_3->axis = glm::vec3(1.f, 0.f, 0.f);
	rigid_cube_3->rotation = 1.5708f;
	m_physics.push_object(rigid_cube_3);
	pbr_obj.push_back(rigid_cube_3);

	Object* rigid_plane_ = new Object(O_PLANE, glm::vec3(1.5f, -52.f, 0.5f), glm::vec3(3.0f, 3.0f, 3.0f), P_DIMENSION);
	m_physics.push_object(rigid_plane_);
	pbr_obj.push_back(rigid_plane_);

	SoftBodyPhysics* sb_sphere = new SoftBodyPhysics(O_SPHERE, glm::vec3(6.5f, 0.f, 2.f), glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION);
	sb_sphere->stiffness = 0.35f;
	sb_sphere->m_mass = 0.5f;
	m_physics.push_object(sb_sphere);
	softbody_obj.push_back(sb_sphere);

	// camera setting
	camera->position = glm::vec3(6.5f, 0.f, 10.f);
	camera->yaw = -90.f;
	camera->pitch = 0.0f;
	camera->zoom = 45.0f;
	// load PBR material textures
	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		// Fabric
		pbr_obj[i]->albedo = albedo[2];
		pbr_obj[i]->normal = normal[2];
		pbr_obj[i]->metallic = metallic[2];
		pbr_obj[i]->roughness = roughness[2];
		pbr_obj[i]->ao = ao[2];
	}
	for (unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		// wood
		softbody_obj[i]->albedo = albedo[4];
		softbody_obj[i]->normal = normal[4];
		softbody_obj[i]->metallic = metallic[4];
		softbody_obj[i]->roughness = roughness[4];
		softbody_obj[i]->ao = ao[4];
		softbody_obj[i]->m_textype = FABRIC;
	}
}
void Scene::Scene1Init(Camera* camera)
{
	// camera setting
	camera->position = glm::vec3(10.f, -2.f, 7.0f);
	camera->yaw = -160.f;
	camera->pitch = 0.0f;
	camera->zoom = 45.0f;

	// Generate objects for scene0
	Object* main_obj_texture = new Object(O_SPHERE, glm::vec3(1.2f,  -2.5f, 4.0f), glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // mid
	// WOOD
	main_obj_texture->albedo = albedo[2];
	main_obj_texture->normal = normal[2];
	main_obj_texture->metallic = metallic[2];
	main_obj_texture->roughness = roughness[2];
	main_obj_texture->ao = ao[2];
	main_obj_texture->m_textype = WOOD;
	m_physics.push_object(main_obj_texture);
	pbr_obj.push_back(main_obj_texture);

	Object* main_obj_texture2 = new Object(O_SPHERE, glm::vec3(1.2f, -0.5f, 2.0f), glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // top
	// STEEL
	main_obj_texture2->albedo = albedo[1];
	main_obj_texture2->normal = normal[1];
	main_obj_texture2->metallic = metallic[1];
	main_obj_texture2->roughness = roughness[1];
	main_obj_texture2->ao = ao[1];
	main_obj_texture2->m_textype = STEEL;
	m_physics.push_object(main_obj_texture2);
	pbr_obj.push_back(main_obj_texture2);

	Object* main_obj_texture3 = new Object(O_SPHERE, glm::vec3(1.2f, -4.5f, 6.0f), glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // bottom
	// GOLD
	main_obj_texture3->albedo = albedo[10];
	main_obj_texture3->normal = normal[10];
	main_obj_texture3->metallic = metallic[10];
	main_obj_texture3->roughness = roughness[10];
	main_obj_texture3->ao = ao[10];
	main_obj_texture3->m_textype = GOLD;
	m_physics.push_object(main_obj_texture3);
	pbr_obj.push_back(main_obj_texture3);

	SoftBodyPhysics* plane = new SoftBodyPhysics(O_PLANE, glm::vec3(0, 1.5f, 1.f), glm::vec3(6.f, 1.f, 10.f), P_DIMENSION);
	plane->m_mass = 0.15f;
	plane->stiffness = 0.8f;
	m_physics.push_object(plane);
	softbody_obj.push_back(plane);

	for (unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		// wood
		softbody_obj[i]->albedo = albedo[7];
		softbody_obj[i]->normal = normal[7];
		softbody_obj[i]->metallic = metallic[7];
		softbody_obj[i]->roughness = roughness[7];
		softbody_obj[i]->ao = ao[7];
		softbody_obj[i]->m_textype = COPPER;
	}
}
void Scene::Scene2Init(Camera* camera)
{
	// camera setting
	camera->position = glm::vec3(2.f, 3.f, 20.0f);
	camera->yaw = -90.f;
	camera->pitch = 0.0f;
	camera->zoom = 45.0f;

	Object* rigid_plane = new Object(O_PLANE, glm::vec3(4.f, 0.5f, -2.f), glm::vec3(10.f, 10.f, 4.f), P_DIMENSION);
	rigid_plane->rotation = 0.5f;
	m_physics.push_object(rigid_plane);
	pbr_obj.push_back(rigid_plane);

	Object* rigid_plane_2 = new Object(O_PLANE, glm::vec3(0.f, 5.f, -10.f), glm::vec3(4.f, 10.f, 10.f), P_DIMENSION);
	rigid_plane_2->axis = glm::vec3(1.f, 0.f, 0.f);
	rigid_plane_2->rotation = 0.5f;
	m_physics.push_object(rigid_plane_2);
	pbr_obj.push_back(rigid_plane_2);

	Object* rigid_plane_3 = new Object(O_PLANE, glm::vec3(-9.f, 5.f, -2.f), glm::vec3(10.f, 10.f, 4.f), P_DIMENSION);
	rigid_plane_3->axis = glm::vec3(0.f, 0.f, -1.f);
	rigid_plane_3->rotation = 0.5f;
	m_physics.push_object(rigid_plane_3);
	pbr_obj.push_back(rigid_plane_3);

	Object* rigid_plane_4 = new Object(O_PLANE, glm::vec3(-13.f, -5.f, -13.f), glm::vec3(30.f, 30.f, 30.f), P_DIMENSION);
	m_physics.push_object(rigid_plane_4);
	pbr_obj.push_back(rigid_plane_4);

	SoftBodyPhysics* sb_sphere = new SoftBodyPhysics(O_SPHERE, glm::vec3(10.f, 6.8f, 0.f), glm::vec3(1.f, 1.f, 1.f), S_DIMENSION);
	m_physics.push_object(sb_sphere);
	softbody_obj.push_back(sb_sphere);

	SoftBodyPhysics* sb_sphere2 = new SoftBodyPhysics(O_SPHERE, glm::vec3(2.f, 7.f, -8.f), glm::vec3(1.f, 1.f, 1.f), S_DIMENSION);
	m_physics.push_object(sb_sphere2);
	softbody_obj.push_back(sb_sphere2);

	SoftBodyPhysics* sb_sphere3 = new SoftBodyPhysics(O_SPHERE, glm::vec3(-8.f, 7.f, 0.f), glm::vec3(1.f, 1.f, 1.f), S_DIMENSION);
	m_physics.push_object(sb_sphere3);
	softbody_obj.push_back(sb_sphere3);

	// load PBR material textures
	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		// wood
		pbr_obj[i]->albedo = albedo[2];
		pbr_obj[i]->normal = normal[2];
		pbr_obj[i]->metallic = metallic[2];
		pbr_obj[i]->roughness = roughness[2];
		pbr_obj[i]->ao = ao[2];
	}
	// steel
	softbody_obj[0]->albedo = albedo[1];
	softbody_obj[0]->normal = normal[1];
	softbody_obj[0]->metallic = metallic[1];
	softbody_obj[0]->roughness = roughness[1];
	softbody_obj[0]->ao = ao[1];
	softbody_obj[0]->m_textype = STEEL;

	// torn_fabric
	softbody_obj[1]->albedo = albedo[5];
	softbody_obj[1]->normal = normal[5];
	softbody_obj[1]->metallic = metallic[5];
	softbody_obj[1]->roughness = roughness[5];
	softbody_obj[1]->ao = ao[5];
	softbody_obj[1]->m_textype = TORN_FABRIC;

	// rusted_iron
	softbody_obj[2]->albedo = albedo[3];
	softbody_obj[2]->normal = normal[3];
	softbody_obj[2]->metallic = metallic[3];
	softbody_obj[2]->roughness = roughness[3];
	softbody_obj[2]->ao = ao[3];
	softbody_obj[2]->m_textype = RUSTED_IRON;
}
void Scene::Scene3Init(Camera* camera)
{
	// camera setting
	camera->position = glm::vec3(3.f, -1.f, 17.0f);
	camera->yaw = -90.f;
	camera->pitch = -19.0f;
	camera->zoom = 45.0f;

	SoftBodyPhysics* sphere[10];
	for (int i = 0; i < 10; ++i)
	{
		sphere[i] = new SoftBodyPhysics(O_SPHERE, glm::vec3(3.f, 3.f+(float)i*5.f, 2.f), glm::vec3(1.f, 1.f, 1.f), S_DIMENSION);
		sphere[i]->stiffness = 0.35f;
		// steel
		sphere[i]->albedo = albedo[8];
		sphere[i]->normal = normal[8];
		sphere[i]->metallic = metallic[8];
		sphere[i]->roughness = roughness[8];
		sphere[i]->ao = ao[8];
		sphere[i]->m_textype = CONCRETE;
		m_physics.push_object(sphere[i]);
		softbody_obj.push_back(sphere[i]);
	}

	//front
	Object* rigid_cube_0 = new Object(O_PLANE, glm::vec3(0.f, -10.f, 7.f), glm::vec3(7.f, 2.f, 2.f), P_DIMENSION);
	rigid_cube_0->axis = glm::vec3(1.f, 0.f, 0.f); 
	rigid_cube_0->rotation = -1.5708f;
	m_physics.push_object(rigid_cube_0);
	pbr_obj.push_back(rigid_cube_0);

	//right
	Object* rigid_cube_1 = new Object(O_PLANE, glm::vec3(7.f, -10.f, 0.f), glm::vec3(2.f, 2.f, 7.f), P_DIMENSION);
	rigid_cube_1->axis = glm::vec3(0.f, 0.f, 1.f);
	rigid_cube_1->rotation = 1.5708f;
	m_physics.push_object(rigid_cube_1);
	pbr_obj.push_back(rigid_cube_1);

	//left
	Object* rigid_cube_2 = new Object(O_PLANE, glm::vec3(0.f, -8.f, 0.f), glm::vec3(2.f, 2.f, 7.f), P_DIMENSION);
	rigid_cube_2->axis = glm::vec3(0.f, 0.f, 1.f);
	rigid_cube_2->rotation = -1.5708f;
	m_physics.push_object(rigid_cube_2);
	pbr_obj.push_back(rigid_cube_2);

	//back
	Object* rigid_cube_3 = new Object(O_PLANE, glm::vec3(0.f, -8.f, 0.f), glm::vec3(7.f, 2.f, 2.f), P_DIMENSION);
	rigid_cube_3->axis = glm::vec3(1.f, 0.f, 0.f);
	rigid_cube_3->rotation = 1.5708f;
	m_physics.push_object(rigid_cube_3);
	pbr_obj.push_back(rigid_cube_3);

	Object* rigid_plane = new Object(O_PLANE, glm::vec3(0.f, -10.f, 0.f), glm::vec3(7.f, 7.f, 7.f), P_DIMENSION);
	m_physics.push_object(rigid_plane);
	pbr_obj.push_back(rigid_plane);

	// load PBR material textures
	for (unsigned i = 0; i < pbr_obj.size(); ++i)
	{
		// wood
		pbr_obj[i]->albedo = albedo[2];
		pbr_obj[i]->normal = normal[2];
		pbr_obj[i]->metallic = metallic[2];
		pbr_obj[i]->roughness = roughness[2];
		pbr_obj[i]->ao = ao[2];
	}
}
void Scene::Scene4Init(Camera* camera)
{
	// camera setting
	camera->yaw = -90.f;
	camera->pitch = 0.0f;
	camera->zoom = 45.0f;
	camera->position = glm::vec3(0.f, 0.f, 4.f);

	glm::vec3 obj_pos = glm::vec3(0.f, 0.f, 0.f);
	Object* pbr_sphere = new Object(O_SPHERE, obj_pos, glm::vec3(1.f, 1.f, 1.f), P_DIMENSION);
	pbr_sphere->albedo = albedo[0];
	pbr_sphere->normal = normal[0];
	pbr_sphere->metallic = metallic[0];
	pbr_sphere->roughness = roughness[0];
	pbr_sphere->ao = ao[0];
	pbr_obj.push_back(pbr_sphere);

	Light m_light;
	m_light.color = glm::vec3(1.f, 1.f, 1.f);
	m_light.position = glm::vec3(0.f, 0.f, -3.f);

	light.push_back(m_light);
	Object* light_ = new Object(O_SPHERE, m_light.position, glm::vec3(0.3f, 0.3f, 0.3f), 10);
	light_obj.push_back(light_);

	angle = 0.f;
}
void Scene::Scene5Init(Camera* camera)
{
	angle = 0.f;
	cam_move = false;
	// camera setting
	camera->yaw = -90.f;
	camera->pitch = 0.0f;
	camera->zoom = 45.0f;
	camera->position = glm::vec3(0.f, 0.f, 0.f);
	int X_SEGMENT = 20, Y_SEGMENT = 20, Z_SEGMENT = 20;
	glm::vec3 temp_coord = glm::vec3(static_cast<float>(-X_SEGMENT), static_cast<float>(-Y_SEGMENT), static_cast<float>(-Z_SEGMENT));
	glm::vec3 init = temp_coord;
	for (int x = -X_SEGMENT + 2; x < X_SEGMENT; x += 2) // -z, z
	{
		temp_coord.x = static_cast<float>(x);
		for (int y = -Y_SEGMENT + 2; y < Y_SEGMENT; y += 2)
		{
			temp_coord.y = static_cast<float>(y);
			int rand_texture = rand() % 11;
			Object* pbr_back = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (-10, -10, -10)
			pbr_back->albedo = albedo[rand_texture];
			pbr_back->normal = normal[rand_texture];
			pbr_back->metallic = metallic[rand_texture];
			pbr_back->roughness = roughness[rand_texture];
			pbr_back->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_back);

			temp_coord.z += (Z_SEGMENT) * 2;
			rand_texture = rand() % 11;
			Object* pbr_front = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (10, -10, 10) ~ (10, -10, 10)
			pbr_front->albedo = albedo[rand_texture];
			pbr_front->normal = normal[rand_texture];
			pbr_front->metallic = metallic[rand_texture];
			pbr_front->roughness = roughness[rand_texture];
			pbr_front->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_front);

			temp_coord.z = static_cast<float>(-Z_SEGMENT);
		}
	}
	temp_coord = init;
	for (int y = -Y_SEGMENT + 2; y < Y_SEGMENT; y += 2) // -y, y
	{
		temp_coord.y = static_cast<float>(y);
		for (int z = -Z_SEGMENT + 2; z < Z_SEGMENT; z += 2)
		{
			temp_coord.z = static_cast<float>(z);
			int rand_texture = rand() % 11;
			Object* pbr_back = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (-10, -10, -10)
			pbr_back->albedo = albedo[rand_texture];
			pbr_back->normal = normal[rand_texture];
			pbr_back->metallic = metallic[rand_texture];
			pbr_back->roughness = roughness[rand_texture];
			pbr_back->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_back);

			temp_coord.x += (X_SEGMENT) * 2;
			rand_texture = rand() % 11;
			Object* pbr_front = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (10, 10, -10) ~ (10, 10, -10)
			pbr_front->albedo = albedo[rand_texture];
			pbr_front->normal = normal[rand_texture];
			pbr_front->metallic = metallic[rand_texture];
			pbr_front->roughness = roughness[rand_texture];
			pbr_front->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_front);

			temp_coord.x = static_cast<float>(-X_SEGMENT);
		}
	}
	temp_coord = init;
	for (int x = -X_SEGMENT + 2; x < X_SEGMENT; x += 2) // -x, x
	{
		temp_coord.x = static_cast<float>(x);
		for (int z = -Z_SEGMENT + 2; z < Z_SEGMENT; z += 2)
		{
			temp_coord.z = static_cast<float>(z);
			int rand_texture = rand() % 11;
			Object* pbr_back = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (-10, -10, -10)
			pbr_back->albedo = albedo[rand_texture];
			pbr_back->normal = normal[rand_texture];
			pbr_back->metallic = metallic[rand_texture];
			pbr_back->roughness = roughness[rand_texture];
			pbr_back->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_back);

			temp_coord.y += (Y_SEGMENT) * 2;
			rand_texture = rand() % 11;
			Object* pbr_front = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, 10, 10) ~ (-10, 10, 10)
			pbr_front->albedo = albedo[rand_texture];
			pbr_front->normal = normal[rand_texture];
			pbr_front->metallic = metallic[rand_texture];
			pbr_front->roughness = roughness[rand_texture];
			pbr_front->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_front);

			temp_coord.y = static_cast<float>(-Y_SEGMENT);
		}
	}

	X_SEGMENT = 10, Y_SEGMENT = 10, Z_SEGMENT = 10;
	for (int x = -X_SEGMENT + 2; x < X_SEGMENT; x += 2) // -z, z
	{
		temp_coord.x = static_cast<float>(x);
		for (int y = -Y_SEGMENT + 2; y < Y_SEGMENT; y += 2)
		{
			temp_coord.y = static_cast<float>(y);
			int rand_texture = rand() % 11;
			Object* pbr_back = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (10, -10, -10)
			pbr_back->albedo = albedo[rand_texture];
			pbr_back->normal = normal[rand_texture];
			pbr_back->metallic = metallic[rand_texture];
			pbr_back->roughness = roughness[rand_texture];
			pbr_back->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_back);

			temp_coord.z += (Z_SEGMENT) * 2;
			rand_texture = rand() % 11;
			Object* pbr_front = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (10, -10, -10)
			pbr_front->albedo = albedo[rand_texture];
			pbr_front->normal = normal[rand_texture];
			pbr_front->metallic = metallic[rand_texture];
			pbr_front->roughness = roughness[rand_texture];
			pbr_front->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_front);

			temp_coord.z = static_cast<float>(-Z_SEGMENT);
		}
	}
	temp_coord = init;
	for (int y = -Y_SEGMENT + 2; y < Y_SEGMENT; y += 2) // -z, z
	{
		temp_coord.y = static_cast<float>(y);
		for (int z = -Z_SEGMENT + 2; z < Z_SEGMENT; z += 2)
		{
			temp_coord.z = static_cast<float>(z);
			int rand_texture = rand() % 11;
			Object* pbr_back = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (10, -10, -10)
			pbr_back->albedo = albedo[rand_texture];
			pbr_back->normal = normal[rand_texture];
			pbr_back->metallic = metallic[rand_texture];
			pbr_back->roughness = roughness[rand_texture];
			pbr_back->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_back);

			temp_coord.x += (X_SEGMENT) * 2;
			rand_texture = rand() % 11;
			Object* pbr_front = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (10, -10, -10)
			pbr_front->albedo = albedo[rand_texture];
			pbr_front->normal = normal[rand_texture];
			pbr_front->metallic = metallic[rand_texture];
			pbr_front->roughness = roughness[rand_texture];
			pbr_front->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_front);

			temp_coord.x = static_cast<float>(-X_SEGMENT);
		}
	}
	temp_coord = init;
	for (int x = -X_SEGMENT + 2; x < X_SEGMENT; x += 2) // -z, z
	{
		temp_coord.x = static_cast<float>(x);
		for (int z = -Z_SEGMENT + 2; z < Z_SEGMENT; z += 2)
		{
			temp_coord.z = static_cast<float>(z);
			int rand_texture = rand() % 11;
			Object* pbr_back = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (10, -10, -10)
			pbr_back->albedo = albedo[rand_texture];
			pbr_back->normal = normal[rand_texture];
			pbr_back->metallic = metallic[rand_texture];
			pbr_back->roughness = roughness[rand_texture];
			pbr_back->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_back);

			temp_coord.y += (Y_SEGMENT) * 2;
			rand_texture = rand() % 11;
			Object* pbr_front = new Object(O_SPHERE, temp_coord, glm::vec3(1.f, 1.f, 1.f), MID_S_DIMENSION); // (-10, -10, -10) ~ (10, -10, -10)
			pbr_front->albedo = albedo[rand_texture];
			pbr_front->normal = normal[rand_texture];
			pbr_front->metallic = metallic[rand_texture];
			pbr_front->roughness = roughness[rand_texture];
			pbr_front->ao = ao[rand_texture];
			pbr_obj.push_back(pbr_front);

			temp_coord.y = static_cast<float>(-Y_SEGMENT);
		}
	}
	// light properties
	for (int i = 0; i < light_num; ++i)
	{
		Light m_light;
		if (i % 4 == 0)
			m_light.color = glm::vec3(1.f, 1.f, 0.1f);
		else if (i % 4 == 1)
			m_light.color = glm::vec3(0.1f, 1.0f, 0.1f);
		else if (i % 4 == 2)
			m_light.color = glm::vec3(0.1f, 0.1f, 0.9f);
		else if (i % 4 == 3)
			m_light.color = glm::vec3(0.9f, 0.9f, 0.9f);
		int x_rand = rand() % 10 - 5;
		int y_rand = rand() % 10 - 5;
		int z_rand = rand() % 10 - 5;
		m_light.position = glm::vec3(x_rand, y_rand, z_rand);

		light.push_back(m_light);
	}
	for (int i = 0; i < light_num; ++i)
	{
		Object* light_ = new Object(O_SPHERE, light[i].position, glm::vec3(0.3f, 0.3f, 0.3f), 10);
		light_obj.push_back(light_);
	}
	magnitude = 3.5f;
}

void Scene::Scene0Draw(GLFWwindow* window, Camera* camera, float dt)
{
	if (dt <= FRAME_LIMIT && move_object)
	{
		if (!softbody_obj.empty())
		{
			m_physics.update(dt);
			for (std::vector<SoftBodyPhysics*>::iterator obj = softbody_obj.begin(); obj != softbody_obj.end(); ++obj)
				(*obj)->Describe();
		}
	}

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	camera->position = softbody_obj[0]->position + glm::vec3(0.f,0.f,8.f);
	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);

	// Draw objs
	DrawObjs(camera, curr_scene);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	// main object metallic, roughness
	pbr_texture_shader.SetFloat("roughness_val", rou);
	pbr_texture_shader.SetBool("roughness_status", roughness_status);
	pbr_texture_shader.SetBool("metallic_status", metallic_status);
	pbr_texture_shader.SetFloat("metallic_val", met);

	// render skybox (render as last to prevent overdraw)
	renderSkybox(&backgroundShader, camera, envCubemap, irradianceMap);
}
void Scene::Scene1Draw(Camera* camera, float dt)
{
	if (dt <= FRAME_LIMIT && move_object)
	{
		if (!softbody_obj.empty())
		{
			m_physics.update(dt);
			for (std::vector<SoftBodyPhysics*>::iterator obj = softbody_obj.begin(); obj != softbody_obj.end(); ++obj)
				(*obj)->Describe();
		}
	}
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);

	// Draw objs
	DrawObjs(camera, curr_scene);

	//pbr_texture_shader.Use();
	//camera->Update(&pbr_texture_shader);
	//pbr_texture_shader.SetVec3("camPos", camera->position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	// main object metallic, roughness
	pbr_texture_shader.SetFloat("roughness_val", rou);
	pbr_texture_shader.SetBool("roughness_status", roughness_status);
	pbr_texture_shader.SetBool("metallic_status", metallic_status);
	pbr_texture_shader.SetFloat("metallic_val", met);

	// render skybox (render as last to prevent overdraw)
	renderSkybox(&backgroundShader, camera, envCubemap, irradianceMap);
}

void Scene::Scene2Draw(Camera* camera, float dt)
{

	if (dt <= FRAME_LIMIT && move_object)
	{
		if (!softbody_obj.empty())
		{
			m_physics.update(dt);
			for (std::vector<SoftBodyPhysics*>::iterator obj = softbody_obj.begin(); obj != softbody_obj.end(); ++obj)
				(*obj)->Describe();
		}
	}
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);

	// Draw objs
	DrawObjs(camera, curr_scene);

	//pbr_texture_shader.Use();
	//camera->Update(&pbr_texture_shader);
	//pbr_texture_shader.SetVec3("camPos", camera->position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	// main object metallic, roughness
	pbr_texture_shader.SetFloat("roughness_val", rou);
	pbr_texture_shader.SetBool("roughness_status", roughness_status);
	pbr_texture_shader.SetBool("metallic_status", metallic_status);
	pbr_texture_shader.SetFloat("metallic_val", met);

	// render skybox (render as last to prevent overdraw)
	renderSkybox(&backgroundShader, camera, envCubemap, irradianceMap);
}
void Scene::Scene3Draw(Camera* camera, float dt)
{
	Scene2Draw(camera, dt);
}

void Scene::Scene4Draw(Camera* camera, float dt)
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);

	// Draw objs
	DrawObjs(camera, curr_scene);
	// update Lighting
	light_obj[0]->color = light[0].color * 300.f;
	pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(0) + "]", light_obj[0]->position);
	pbr_texture_shader.SetVec3("lightColors[" + std::to_string(0) + "]", light_obj[0]->color);

	lightShader.Use();
	light_obj[0]->position.x = sinf(angle) * 3.f;
	light_obj[0]->position.z = cosf(angle) * 3.f;
	light[0].position = light_obj[0]->position;
	//light[0].color = light_obj[0]->color * 300.f;
	angle += dt;
	lightShader.SetVec3("lightPosition", light[0].position);
	lightShader.SetVec3("lightColor", light[0].color);
	light_obj[0]->render_lights(camera, &lightShader, light_obj[0]->position, aspect);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	pbr_texture_shader.Use();
	// main object metallic, roughness
	pbr_texture_shader.SetFloat("roughness_val", rou);
	pbr_texture_shader.SetBool("roughness_status", roughness_status);
	pbr_texture_shader.SetBool("metallic_status", metallic_status);
	pbr_texture_shader.SetFloat("metallic_val", met);
	
	// render skybox (render as last to prevent overdraw)
	renderSkybox(&backgroundShader, camera, envCubemap, irradianceMap);
}
void Scene::Scene5Draw(Camera* camera, float dt)
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pbr_texture_shader.Use();
	camera->Update(&pbr_texture_shader);
	pbr_texture_shader.SetVec3("camPos", camera->position);

	// Draw objs
	DrawObjs(camera, curr_scene);

	// lighting
	for (unsigned int i = 0; i < light_num; ++i)
	{
		light_obj[i]->color = light[i].color * 300.f;
		pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", light_obj[i]->position);
		pbr_texture_shader.SetVec3("lightColors[" + std::to_string(i) + "]", light_obj[i]->color); // 300, 300, 300
	}
	// lighting
	lightShader.Use();
	glm::vec3 prev = light_obj[cam_num]->position;
	for (unsigned int i = 1; i < light_num; ++i)
	{
		light_obj[i]->position.x = (i * (1 + sin(i * 2 * PI / 25 * angle * 3)) * sin(PI / (10 + i) * (2 + sin(2 * i * PI / 35 * angle * 3))) * cos(i * PI * (1 + sin(2 * PI / 35 * angle * 3)))) * magnitude;
		light_obj[i]->position.y = (4 + sin(i * PI / 25 * angle * 3)) * cos(PI / 4 * (2 + sin(i * 2 * PI / 35 * angle * 3))) * magnitude;
		light_obj[i]->position.z = (4 + sin(i * PI / 25 * angle * 3)) * sin(PI / (10 + i) * (2 + sin(2 * PI / 35 * angle * 3))) * sin(i * PI * (1 + sin(2 * PI / 35 * angle * 3))) * magnitude;

		angle += (orbit_speed / light_num);
		lightShader.SetVec3("lightPosition", light[i].position);
		lightShader.SetVec3("lightColor", light[i].color);
		light[i].position = light_obj[i]->position;

		if (i != cam_num)
			light_obj[i]->render_lights(camera, &lightShader, light_obj[i]->position, aspect);
	}

	if (cam_move)
	{
		glm::vec3 look_vec = light_obj[cam_num]->position - prev;
		float dist = glm::distance(light_obj[cam_num]->position, prev);
		camera->pitch = glm::degrees(asinf(look_vec.y / dist));
		camera->yaw = glm::degrees(asinf(look_vec.z / dist));
		if (dist < 0.03f)
			dist = 0.03f;
		camera->position += (dist * glm::normalize(look_vec));
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	// main object metallic, roughness
	pbr_texture_shader.SetBool("roughness_status", true);
	pbr_texture_shader.SetBool("metallic_status", true);

	// render skybox (render as last to prevent overdraw)
	renderSkybox(&backgroundShader, camera, envCubemap, irradianceMap);
}
void Scene::DrawObjs(Camera* camera, unsigned scene_num)
{
	// bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

	for (auto obj : pbr_obj)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, (*obj).albedo);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, (*obj).normal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, (*obj).metallic);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, (*obj).roughness);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, (*obj). ao);
	}
	for (std::vector<Object*>::iterator p_obj = pbr_obj.begin(); p_obj != pbr_obj.end(); ++p_obj)
	{
		pbr_texture_shader.SetInt("albedoMap", (*p_obj)->albedo + 2);
		pbr_texture_shader.SetInt("normalMap", (*p_obj)->normal + 2);
		pbr_texture_shader.SetInt("metallicMap", (*p_obj)->metallic + 2);
		pbr_texture_shader.SetInt("roughnessMap", (*p_obj)->roughness + 2);
		pbr_texture_shader.SetInt("aoMap", (*p_obj)->ao + 2);
		(*p_obj)->render_objs(camera, &pbr_texture_shader, (*p_obj)->position, aspect, false);
	}
	for(unsigned i = 0; i < softbody_obj.size(); ++i)
	{
		int buff_offset = ChangePBRTexture(softbody_obj[i]->m_textype, i, true);
		glActiveTexture(GL_TEXTURE3 + buff_offset);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->albedo);
		glActiveTexture(GL_TEXTURE4 + buff_offset);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->normal);
		glActiveTexture(GL_TEXTURE5 + buff_offset);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->metallic);
		glActiveTexture(GL_TEXTURE6 + buff_offset);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->roughness);
		glActiveTexture(GL_TEXTURE7 + buff_offset);
		glBindTexture(GL_TEXTURE_2D, softbody_obj[i]->ao);
	}
	for (std::vector<SoftBodyPhysics*>::iterator s_obj = softbody_obj.begin(); s_obj != softbody_obj.end(); ++s_obj)
	{
		pbr_texture_shader.SetInt("albedoMap", (*s_obj)->albedo + 2);
		pbr_texture_shader.SetInt("normalMap", (*s_obj)->normal + 2);
		pbr_texture_shader.SetInt("metallicMap", (*s_obj)->metallic + 2);
		pbr_texture_shader.SetInt("roughnessMap", (*s_obj)->roughness + 2);
		pbr_texture_shader.SetInt("aoMap", (*s_obj)->ao + 2);
		(*s_obj)->render_objs(camera, &pbr_texture_shader, (*s_obj)->position, aspect, draw_line);
	}
	pbr_texture_shader.SetInt("light_num", static_cast<int>(light_obj.size()));
}
void Scene::DeletePBRTextures()
{
	for (unsigned i = 0; i < pbr_number; ++i)
	{
		glDeleteTextures(1, &albedo[i]);
		glDeleteTextures(1, &normal[i]);
		glDeleteTextures(1, &roughness[i]);
		glDeleteTextures(1, &metallic[i]);
		glDeleteTextures(1, &ao[i]);
	}
}
void Scene::ImGuiUpdate(GLFWwindow* window, Camera* camera, float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	bool show_demo_window = false;

	ImGui::Begin("GUI interface");
	ImGui::Text("Frame Per Second : %d ms", static_cast<int>(1.f / dt));
	ImGui::End();

	if (second_imgui)
	{
		ImGui::Begin("Scene selector");
		if (ImGui::Button("Reload"))
			Reload(camera);
		if (ImGui::Button("Scene0"))
		{
			if (curr_scene != 0)
			{
				ShutDown();
				m_physics.clear_objects();
				curr_scene = 0;
				Init(window, camera);
			}
		}
		if (ImGui::Button("Scene1"))
		{
			if (curr_scene != 1)
			{
				ShutDown();
				m_physics.clear_objects();
				curr_scene = 1;
				Init(window, camera);
			}
		}
		if (ImGui::Button("Scene2"))
		{
			if (curr_scene != 2)
			{
				ShutDown();
				m_physics.clear_objects();
				curr_scene = 2;
				Init(window, camera);
			}
		}
		if (ImGui::Button("Scene3"))
		{
			if (curr_scene != 3)
			{
				ShutDown();
				m_physics.clear_objects();
				curr_scene = 3;
				Init(window, camera);
			}
		}
		if (ImGui::Button("Scene4"))
		{
			if (curr_scene != 4)
			{
				ShutDown();
				// no physics obj
				curr_scene = 4;
				Init(window, camera);
			}
		}
		if (ImGui::Button("Scene5"))
		{
			if (curr_scene != 5)
			{
				ShutDown();
				// no physics obj
				curr_scene = 5;
				Init(window, camera);
			}
		}
		ImGui::End();
	}
	if (third_imgui)
	{
		ImGui::Begin("Select PBR texture");
		
		unsigned sz = static_cast<unsigned>(softbody_obj.size() + 1);
		bool for_softbody = (curr_scene == 4 ? false : true);
		if (ImGui::Button("Plastic"))
			ChangePBRTexture(PLASTIC, sz, for_softbody);
		if (ImGui::Button("Steel"))
			ChangePBRTexture(STEEL, sz, for_softbody);
		if (ImGui::Button("Wood"))
			ChangePBRTexture(WOOD, sz, for_softbody);
		if (ImGui::Button("Rusted-Iron"))
			ChangePBRTexture(RUSTED_IRON, sz, for_softbody);
		if (ImGui::Button("Fabric"))
			ChangePBRTexture(FABRIC, sz, for_softbody);
		if (ImGui::Button("TornFabric"))
			ChangePBRTexture(TORN_FABRIC, sz, for_softbody);
		if (ImGui::Button("Aluminium"))
			ChangePBRTexture(ALUMINIUM, sz, for_softbody);
		if (ImGui::Button("Copper"))
			ChangePBRTexture(COPPER, sz, for_softbody);
		if (ImGui::Button("Concrete"))
			ChangePBRTexture(CONCRETE, sz, for_softbody);
		if (ImGui::Button("Leather"))
			ChangePBRTexture(LEATHER, sz, for_softbody);
		if (ImGui::Button("Gold"))
			ChangePBRTexture(GOLD, sz, for_softbody);
		ImGui::End();
	}
	if (forth_imgui)
	{
		ImGui::Begin("Draw by Texture / Line");
		if (ImGui::Button("Draw Line"))
			draw_line = true;
		if (ImGui::Button("Draw Texture"))
			draw_line = false;
		ImGui::End();
	}
	if (curr_scene == 5)
	{
		ImGui::Begin("Camera properties");
		if (fifth_imgui)
			fifth_imgui = false;
		if (cam_move)
		{
			ImGui::Text("Current Light Num : %d out of %d", cam_num, light_num);
			if (ImGui::Button("Another travel"))
			{
				++cam_num;
				if (cam_num >= light_num)
					cam_num = 1;
				camera->position = light_obj[cam_num]->position;
			}
		}
		if (!cam_move)
		{
			if (ImGui::Button("Let's Travel"))
			{
				camera->position = light_obj[cam_num]->position;
				cam_move = true;
			}
		}
		else
		{
			if (ImGui::Button("Camera stick to the point"))
			{
				camera->yaw = -90.f;
				camera->pitch = 0.0f;
				camera->zoom = 45.0f;
				camera->position = glm::vec3(0.f, 0.f, 0.f);
				cam_move = false;
			}
		}
		ImGui::End();
	}
	if (curr_scene == 0 || curr_scene == 1 || curr_scene == 2 || curr_scene == 3)
	{
		ImGui::Begin("Object Movement");
		if (!fifth_imgui)
			fifth_imgui = true;
		if (move_object)
		{
			if (ImGui::Button("Stop"))
				move_object = false;
		}
		else
		{
			if (ImGui::Button("Move"))
				move_object = true;
		}
		ImGui::End();

		float stiffness = softbody_obj[0]->stiffness;
		float newstiffness = stiffness;
		ImGui::Begin("Soft Body");
		ImGui::SliderFloat("Stiffness", &newstiffness, 0.1f, 0.5f);

		ImGui::End();

		if (stiffness != newstiffness)// || damping != newdamping)
		{
			for (size_t i = 0; i < softbody_obj.size(); ++i)
			{
				softbody_obj[i]->stiffness = newstiffness;
				//softbody_obj[i]->damping = newdamping;
			}
		}
	}
	if (fifth_imgui)
	{
		ImGui::Begin("Roughness / Metalic Controller");
		if (roughness_status)
		{
			if (ImGui::Button("Default roughness"))
				roughness_status = false;
			ImGui::SliderFloat("roughness", &rou, 0.f, 1.f);
		}
		else
		{
			if (ImGui::Button("Control roughness"))
				roughness_status = true;
		}
		if (metallic_status)
		{
			if (ImGui::Button("Default metallic"))
				metallic_status = false;
			ImGui::SliderFloat("metallic", &met, 0.f, 1.f);
		}
		else
		{
			if (ImGui::Button("Control metallic"))
				metallic_status = true;
		}
		ImGui::End();
	}
	if (curr_scene == 5)
	{
		third_imgui = false;
		forth_imgui = false;
		fifth_imgui = false;
	}
	else if (curr_scene == 4)
	{
		third_imgui = true;
		forth_imgui = false;
		fifth_imgui = true;
		ImGui::Begin("Light Properties");
		ImGui::ColorEdit3("Light Color", (float*)&light[0].color);
		light_obj[0]->color = light[0].color;
		ImGui::End();
	}
	else
	{
		third_imgui = true;
		forth_imgui = true;
		fifth_imgui = true;
	}
}
void Scene::ImGuirender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void Scene::ImGuiShutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void Scene::Reload(Camera* camera)
{
	ShutDown();
	if (curr_scene == 0)
		Scene0Init(camera);
	else if (curr_scene == 1)
		Scene1Init(camera);
	else if (curr_scene == 2)
		Scene2Init(camera);
	else if (curr_scene == 3)
		Scene3Init(camera);
	else if (curr_scene == 4)
		Scene4Init(camera);
	else if (curr_scene == 5)
		Scene5Init(camera);
}
void Scene::ShutDown()
{
	for (auto p_obj : pbr_obj)
	{
		delete p_obj;
		p_obj = NULL;
	}
	if(!pbr_obj.empty())
		pbr_obj.clear();
	for (auto s_obj : softbody_obj)
	{
		delete s_obj;
		s_obj = NULL;
	}
	if(!softbody_obj.empty())
		softbody_obj.clear();
	m_physics.clear_objects();
	if(!light.empty())
		light.clear();
	for (auto l_obj : light_obj)
	{
		delete l_obj;
		l_obj = NULL;
	}
	if (!light_obj.empty())
		light_obj.clear();
}
void Scene::DeleteBuffers()
{
	if (captureFBO)
		glDeleteFramebuffers(1, &captureFBO);
	if (captureRBO)
		glDeleteRenderbuffers(1, &captureRBO);
	if (envCubemap)
		glDeleteTextures(1, &envCubemap);
	if (irradianceMap)
		glDeleteTextures(1, &irradianceMap);
	if (prefilterMap)
		glDeleteTextures(1, &prefilterMap);
	if (brdfLUTTexture)
		glDeleteTextures(1, &brdfLUTTexture);
	for (unsigned i = 0; i < pbr_number; ++i)
	{
		glDeleteTextures(1, &albedo[i]);
		glDeleteTextures(1, &normal[i]);
		glDeleteTextures(1, &metallic[i]);
		glDeleteTextures(1, &roughness[i]);
		glDeleteTextures(1, &ao[i]);
	}
}
void Scene::ResizeFrameBuffer(GLFWwindow* window)
{
	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);
}
void Scene::InitAllPBRTexture()
{
	Object* rigid_plane = new Object(O_PLANE, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), dimension_);

	// plastic
	albedo[0] = rigid_plane->loadTexture("models\\pbr\\plastic\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[0] = rigid_plane->loadTexture("models\\pbr\\plastic\\Sphere3D_1_defaultMat_Normal.png");
	metallic[0] = rigid_plane->loadTexture("models\\pbr\\plastic\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[0] = rigid_plane->loadTexture("models\\pbr\\plastic\\Sphere3D_1_defaultMat_Roughness.png");
	ao[0] = rigid_plane->loadTexture("models\\pbr\\plastic\\ao.png");

	// steel
	albedo[1] = rigid_plane->loadTexture("models\\pbr\\steel\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[1] = rigid_plane->loadTexture("models\\pbr\\steel\\Sphere3D_1_defaultMat_Normal.png");
	metallic[1] = rigid_plane->loadTexture("models\\pbr\\steel\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[1] = rigid_plane->loadTexture("models\\pbr\\steel\\Sphere3D_1_defaultMat_Roughness.png");
	ao[1] = rigid_plane->loadTexture("models\\pbr\\steel\\ao.png");

	// wood
	albedo[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Normal.png");
	metallic[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\Sphere3D_1_defaultMat_Roughness.png");
	ao[2] = rigid_plane->loadTexture("models\\pbr\\Wood\\ao.png");

	// rusted_iron
	albedo[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\Sphere3D_1_defaultMat_Normal.png");
	metallic[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\Sphere3D_1_defaultMat_Roughness.png");
	ao[3] = rigid_plane->loadTexture("models\\pbr\\rusted-iron\\ao.png");

	// fabric
	albedo[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\Sphere3D_1_defaultMat_Normal.png");
	metallic[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\Sphere3D_1_defaultMat_Roughness.png");
	ao[4] = rigid_plane->loadTexture("models\\pbr\\Fabric\\ao.png");

	// torn-fabric
	albedo[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\Sphere3D_1_defaultMat_Normal.png");
	metallic[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\Sphere3D_1_defaultMat_Roughness.png");
	ao[5] = rigid_plane->loadTexture("models\\pbr\\TornFabric\\ao.png");

	// aluminium
	albedo[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\Sphere3D_1_defaultMat_Normal.png");
	metallic[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\Sphere3D_1_defaultMat_Roughness.png");
	ao[6] = rigid_plane->loadTexture("models\\pbr\\Aluminium\\ao.png");

	// copper
	albedo[7] = rigid_plane->loadTexture("models\\pbr\\Copper\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[7] = rigid_plane->loadTexture("models\\pbr\\Copper\\Sphere3D_1_defaultMat_Normal.png");
	metallic[7] = rigid_plane->loadTexture("models\\pbr\\Copper\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[7] = rigid_plane->loadTexture("models\\pbr\\Copper\\Sphere3D_1_defaultMat_Roughness.png");
	ao[7] = rigid_plane->loadTexture("models\\pbr\\Copper\\ao.png");

	// concrete
	albedo[8] = rigid_plane->loadTexture("models\\pbr\\Concrete\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[8] = rigid_plane->loadTexture("models\\pbr\\Concrete\\Sphere3D_1_defaultMat_Normal.png");
	metallic[8] = rigid_plane->loadTexture("models\\pbr\\Concrete\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[8] = rigid_plane->loadTexture("models\\pbr\\Concrete\\Sphere3D_1_defaultMat_Roughness.png");
	ao[8] = rigid_plane->loadTexture("models\\pbr\\Concrete\\ao.png");

	// leather
	albedo[9] = rigid_plane->loadTexture("models\\pbr\\Leather\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[9] = rigid_plane->loadTexture("models\\pbr\\Leather\\Sphere3D_1_defaultMat_Normal.png");
	metallic[9] = rigid_plane->loadTexture("models\\pbr\\Leather\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[9] = rigid_plane->loadTexture("models\\pbr\\Leather\\Sphere3D_1_defaultMat_Roughness.png");
	ao[9] = rigid_plane->loadTexture("models\\pbr\\Leather\\ao.png");

	// gold
	albedo[10] = rigid_plane->loadTexture("models\\pbr\\gold\\Sphere3D_1_defaultMat_BaseColor.png");
	normal[10] = rigid_plane->loadTexture("models\\pbr\\gold\\Sphere3D_1_defaultMat_Normal.png");
	metallic[10] = rigid_plane->loadTexture("models\\pbr\\gold\\Sphere3D_1_defaultMat_Metallic.png");
	roughness[10] = rigid_plane->loadTexture("models\\pbr\\gold\\Sphere3D_1_defaultMat_Roughness.png");
	ao[10] = rigid_plane->loadTexture("models\\pbr\\gold\\ao.png");

	delete rigid_plane;
}

int Scene::ChangePBRTexture(TextureType type, unsigned index, bool isSoftbodyObj)
{
	int to_return = 0;
	if (isSoftbodyObj)
	{
		if (index > softbody_obj.size())
		{
			// change all text
			for (auto obj : softbody_obj)
			{
				if (type == PLASTIC)
				{
					(*obj).albedo = albedo[0];
					(*obj).normal = normal[0];
					(*obj).metallic = metallic[0];
					(*obj).roughness = roughness[0];
					(*obj).ao = ao[0];
					(*obj).m_textype = PLASTIC;
				}
				else if (type == STEEL)
				{
					(*obj).albedo = albedo[1];
					(*obj).normal = normal[1];
					(*obj).metallic = metallic[1];
					(*obj).roughness = roughness[1];
					(*obj).ao = ao[1];
					(*obj).m_textype = STEEL;
					to_return = 5;
				}
				else if (type == WOOD)
				{
					(*obj).albedo = albedo[2];
					(*obj).normal = normal[2];
					(*obj).metallic = metallic[2];
					(*obj).roughness = roughness[2];
					(*obj).ao = ao[2];
					(*obj).m_textype = WOOD;
					to_return = 10;
				}
				else if (type == RUSTED_IRON)
				{
					(*obj).albedo = albedo[3];
					(*obj).normal = normal[3];
					(*obj).metallic = metallic[3];
					(*obj).roughness = roughness[3];
					(*obj).ao = ao[3];
					(*obj).m_textype = RUSTED_IRON;
					to_return = 15;
				}
				else if (type == FABRIC)
				{
					(*obj).albedo = albedo[4];
					(*obj).normal = normal[4];
					(*obj).metallic = metallic[4];
					(*obj).roughness = roughness[4];
					(*obj).ao = ao[4];
					(*obj).m_textype = FABRIC;
					to_return = 20;
				}
				else if (type == TORN_FABRIC)
				{
					(*obj).albedo = albedo[5];
					(*obj).normal = normal[5];
					(*obj).metallic = metallic[5];
					(*obj).roughness = roughness[5];
					(*obj).ao = ao[5];
					(*obj).m_textype = TORN_FABRIC;
					to_return = 25;
				}
				else if (type == ALUMINIUM)
				{
					(*obj).albedo = albedo[6];
					(*obj).normal = normal[6];
					(*obj).metallic = metallic[6];
					(*obj).roughness = roughness[6];
					(*obj).ao = ao[6];
					(*obj).m_textype = ALUMINIUM;
					to_return = 30;
				}
				else if (type == COPPER)
				{
					(*obj).albedo = albedo[7];
					(*obj).normal = normal[7];
					(*obj).metallic = metallic[7];
					(*obj).roughness = roughness[7];
					(*obj).ao = ao[7];
					(*obj).m_textype = COPPER;
					to_return = 35;
				}
				else if (type == CONCRETE)
				{
					(*obj).albedo = albedo[8];
					(*obj).normal = normal[8];
					(*obj).metallic = metallic[8];
					(*obj).roughness = roughness[8];
					(*obj).ao = ao[8];
					(*obj).m_textype = CONCRETE;
					to_return = 40;
				}
				else if (type == LEATHER)
				{
					(*obj).albedo = albedo[9];
					(*obj).normal = normal[9];
					(*obj).metallic = metallic[9];
					(*obj).roughness = roughness[9];
					(*obj).ao = ao[9];
					(*obj).m_textype = LEATHER;
					to_return = 45;
				}
				else if (type == GOLD)
				{
					(*obj).albedo = albedo[10];
					(*obj).normal = normal[10];
					(*obj).metallic = metallic[10];
					(*obj).roughness = roughness[10];
					(*obj).ao = ao[10];
					(*obj).m_textype = GOLD;
					to_return = 50;
				}
			}
		}
		else
		{
			if (type == PLASTIC)
			{
				softbody_obj[index]->albedo = albedo[0];
				softbody_obj[index]->normal = normal[0];
				softbody_obj[index]->metallic = metallic[0];
				softbody_obj[index]->roughness = roughness[0];
				softbody_obj[index]->ao = ao[0];
				softbody_obj[index]->m_textype = PLASTIC;
			}
			else if (type == STEEL)
			{
				softbody_obj[index]->albedo = albedo[1];
				softbody_obj[index]->normal = normal[1];
				softbody_obj[index]->metallic = metallic[1];
				softbody_obj[index]->roughness = roughness[1];
				softbody_obj[index]->ao = ao[1];
				softbody_obj[index]->m_textype = STEEL;
				to_return = 5;
			}
			else if (type == WOOD)
			{
				softbody_obj[index]->albedo = albedo[2];
				softbody_obj[index]->normal = normal[2];
				softbody_obj[index]->metallic = metallic[2];
				softbody_obj[index]->roughness = roughness[2];
				softbody_obj[index]->ao = ao[2];
				softbody_obj[index]->m_textype = WOOD;
				to_return = 10;
			}
			else if (type == RUSTED_IRON)
			{
				softbody_obj[index]->albedo = albedo[3];
				softbody_obj[index]->normal = normal[3];
				softbody_obj[index]->metallic = metallic[3];
				softbody_obj[index]->roughness = roughness[3];
				softbody_obj[index]->ao = ao[3];
				softbody_obj[index]->m_textype = RUSTED_IRON;
				to_return = 15;
			}
			else if (type == FABRIC)
			{
				softbody_obj[index]->albedo = albedo[4];
				softbody_obj[index]->normal = normal[4];
				softbody_obj[index]->metallic = metallic[4];
				softbody_obj[index]->roughness = roughness[4];
				softbody_obj[index]->ao = ao[4];
				softbody_obj[index]->m_textype = FABRIC;
				to_return = 20;
			}
			else if (type == TORN_FABRIC)
			{
				softbody_obj[index]->albedo = albedo[5];
				softbody_obj[index]->normal = normal[5];
				softbody_obj[index]->metallic = metallic[5];
				softbody_obj[index]->roughness = roughness[5];
				softbody_obj[index]->ao = ao[5];
				softbody_obj[index]->m_textype = TORN_FABRIC;
				to_return = 25;
			}
			else if (type == ALUMINIUM)
			{
				softbody_obj[index]->albedo = albedo[6];
				softbody_obj[index]->normal = normal[6];
				softbody_obj[index]->metallic = metallic[6];
				softbody_obj[index]->roughness = roughness[6];
				softbody_obj[index]->ao = ao[6];
				softbody_obj[index]->m_textype = ALUMINIUM;
				to_return = 30;
			}
			else if (type == COPPER)
			{
				softbody_obj[index]->albedo = albedo[7];
				softbody_obj[index]->normal = normal[7];
				softbody_obj[index]->metallic = metallic[7];
				softbody_obj[index]->roughness = roughness[7];
				softbody_obj[index]->ao = ao[7];
				softbody_obj[index]->m_textype = COPPER;
				to_return = 35;
			}
			else if (type == CONCRETE)
			{
				softbody_obj[index]->albedo = albedo[8];
				softbody_obj[index]->normal = normal[8];
				softbody_obj[index]->metallic = metallic[8];
				softbody_obj[index]->roughness = roughness[8];
				softbody_obj[index]->ao = ao[8];
				softbody_obj[index]->m_textype = CONCRETE;
				to_return = 40;
			}
			else if (type == LEATHER)
			{
				softbody_obj[index]->albedo = albedo[9];
				softbody_obj[index]->normal = normal[9];
				softbody_obj[index]->metallic = metallic[9];
				softbody_obj[index]->roughness = roughness[9];
				softbody_obj[index]->ao = ao[9];
				softbody_obj[index]->m_textype = LEATHER;
				to_return = 45;
			}
			else if (type == GOLD)
			{
				softbody_obj[index]->albedo = albedo[10];
				softbody_obj[index]->normal = normal[10];
				softbody_obj[index]->metallic = metallic[10];
				softbody_obj[index]->roughness = roughness[10];
				softbody_obj[index]->ao = ao[10];
				softbody_obj[index]->m_textype = GOLD;
				to_return = 50;
			}
		}
	}
	else
	{
	// change all text
	for (auto obj : pbr_obj)
	{
		if (type == PLASTIC)
		{
			(*obj).albedo = albedo[0];
			(*obj).normal = normal[0];
			(*obj).metallic = metallic[0];
			(*obj).roughness = roughness[0];
			(*obj).ao = ao[0];
			(*obj).m_textype = PLASTIC;
		}
		else if (type == STEEL)
		{
			(*obj).albedo = albedo[1];
			(*obj).normal = normal[1];
			(*obj).metallic = metallic[1];
			(*obj).roughness = roughness[1];
			(*obj).ao = ao[1];
			(*obj).m_textype = STEEL;
			to_return = 5;
		}
		else if (type == WOOD)
		{
			(*obj).albedo = albedo[2];
			(*obj).normal = normal[2];
			(*obj).metallic = metallic[2];
			(*obj).roughness = roughness[2];
			(*obj).ao = ao[2];
			(*obj).m_textype = WOOD;
			to_return = 10;
		}
		else if (type == RUSTED_IRON)
		{
			(*obj).albedo = albedo[3];
			(*obj).normal = normal[3];
			(*obj).metallic = metallic[3];
			(*obj).roughness = roughness[3];
			(*obj).ao = ao[3];
			(*obj).m_textype = RUSTED_IRON;
			to_return = 15;
		}
		else if (type == FABRIC)
		{
			(*obj).albedo = albedo[4];
			(*obj).normal = normal[4];
			(*obj).metallic = metallic[4];
			(*obj).roughness = roughness[4];
			(*obj).ao = ao[4];
			(*obj).m_textype = FABRIC;
			to_return = 20;
		}
		else if (type == TORN_FABRIC)
		{
			(*obj).albedo = albedo[5];
			(*obj).normal = normal[5];
			(*obj).metallic = metallic[5];
			(*obj).roughness = roughness[5];
			(*obj).ao = ao[5];
			(*obj).m_textype = TORN_FABRIC;
			to_return = 25;
		}
		else if (type == ALUMINIUM)
		{
			(*obj).albedo = albedo[6];
			(*obj).normal = normal[6];
			(*obj).metallic = metallic[6];
			(*obj).roughness = roughness[6];
			(*obj).ao = ao[6];
			(*obj).m_textype = ALUMINIUM;
			to_return = 30;
		}
		else if (type == COPPER)
		{
			(*obj).albedo = albedo[7];
			(*obj).normal = normal[7];
			(*obj).metallic = metallic[7];
			(*obj).roughness = roughness[7];
			(*obj).ao = ao[7];
			(*obj).m_textype = COPPER;
			to_return = 35;
		}
		else if (type == CONCRETE)
		{
			(*obj).albedo = albedo[8];
			(*obj).normal = normal[8];
			(*obj).metallic = metallic[8];
			(*obj).roughness = roughness[8];
			(*obj).ao = ao[8];
			(*obj).m_textype = CONCRETE;
			to_return = 40;
		}
		else if (type == LEATHER)
		{
			(*obj).albedo = albedo[9];
			(*obj).normal = normal[9];
			(*obj).metallic = metallic[9];
			(*obj).roughness = roughness[9];
			(*obj).ao = ao[9];
			(*obj).m_textype = LEATHER;
			to_return = 45;
		}
		else if (type == GOLD)
		{
			(*obj).albedo = albedo[10];
			(*obj).normal = normal[10];
			(*obj).metallic = metallic[10];
			(*obj).roughness = roughness[10];
			(*obj).ao = ao[10];
			(*obj).m_textype = GOLD;
			to_return = 50;
		}
	}
	}
	return to_return;
}