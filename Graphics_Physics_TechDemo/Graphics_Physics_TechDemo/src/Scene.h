#pragma once

#ifndef SCENE_H
#define SCENE_H

#include "Object.h"
#include "Base.h"
#include "Physics.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "imgui-master\imgui.h"
#include "imgui-master\imgui_impl_glfw.h"
#include "imgui-master\imgui_impl_opengl3.h"
#include "GLFW\glfw3.h"

const unsigned pbr_number = 11;
const unsigned light_num = 20;

class Scene {
public:
	Scene(int sceneNum) : curr_scene(sceneNum), width(1280), height(800), aspect(1280.f/800.f),
		roughness_status(false), metallic_status(false), dimension_(64), met(0.f), rou(0.f),
		second_imgui(true), third_imgui(true), forth_imgui(true), deltaTime(0.f), lastFrame(0.f), draw_line(false),
		textIndex(0), cam_num(5), cam_move(false){};
	~Scene() {};
	void Init(GLFWwindow* window, Camera* camera);
	void Update(GLFWwindow* window, Camera* camera, float dt);

	void Scene0Init(Camera* camera);
	void Scene1Init(Camera* camera);
	void Scene2Init(Camera* camera);
	void Scene3Init(Camera* camera);
	void Scene4Init(Camera* camera);

	void Scene0Draw(Camera* camera, float dt);
	void Scene1Draw(Camera* camera, float dt);
	void Scene2Draw(Camera* camera, float dt);
	void Scene3Draw(Camera* camera, float dt);
	void Scene4Draw(Camera* camera, float dt);

	void ShutDown();
	void DeleteBuffers();
	void DrawObjs(Camera* camera, unsigned scene_num);

	void InitAllPBRTexture();
	int ChangePBRTexture(TextureType type, unsigned index);
	void Reload(Camera* camera);

	void ImGuiUpdate(GLFWwindow* window, Camera* camera, float dt);
	void ImGuirender();
	void ImGuiShutdown();

	void ResizeFrameBuffer(GLFWwindow* window);
private:
	std::vector<Object*> pbr_obj;
	std::vector<Object*> light_obj;
	std::vector<SoftBodyPhysics*> softbody_obj;
	std::vector<Light> light;

	void push_object(Object* _obj) { pbr_obj.push_back(_obj); }
	void push_softbody_object(SoftBodyPhysics* _obj) { softbody_obj.push_back(_obj); }
	unsigned curr_scene;
	int width, height;
	float aspect;

	Physics m_physics;

	Shader pbrshader;
	Shader pbr_texture_shader;
	Shader equirectangularToCubmapShader;
	Shader irradianceShader;
	Shader backgroundShader;
	Shader prefilterShader;
	Shader brdfShader;
	Shader lightShader;

	unsigned int captureFBO = 0;
	unsigned int captureRBO = 0;
	unsigned int envCubemap = 0;
	unsigned int irradianceMap = 0;
	unsigned int prefilterMap = 0;
	unsigned int brdfLUTTexture = 0;

	unsigned int albedo[pbr_number] = { 0 };
	unsigned int normal[pbr_number] = { 0 };
	unsigned int metallic[pbr_number] = { 0 };
	unsigned int roughness[pbr_number] = { 0 };
	unsigned int ao[pbr_number] = { 0 };

	int dimension_;

	float orbit_speed = 0.0003f;
	float angle = 0.f;
	float magnitude = 10.f;

	float met = 0.f;
	float rou = 0.f;
	float time = 0.f;

	bool roughness_status;
	bool metallic_status;
	bool cam_move;

	bool second_imgui;
	bool third_imgui;
	bool forth_imgui;

	float deltaTime;
	float lastFrame;

	unsigned textIndex;
	unsigned cam_num;

	bool draw_line;
};


#endif