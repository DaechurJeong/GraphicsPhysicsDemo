/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: Phisically based rendering for graphics and 3D physics
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TecDemo
Author: Charlie Jung, jungdae.chur
Author2: Nahye Park, nahye.park
Creation date: 9/6/2019
End Header --------------------------------------------------------*/
#include "Shader.h"
#include "GLFW\glfw3.h"
#include "input.h"
#include "Camera.h"
#include "Light.h"
#include "Base.h"
#include "Physics.h"
#include "imgui-master\imgui.h"
#include "imgui-master\imgui_impl_glfw.h"
#include "imgui-master\imgui_impl_opengl3.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>

// Global variables
int width = 1280, height = 800;
float deltaTime, lastFrame = 0.f;
float aspect = float(width) / float(height);
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;
glm::vec3 middlePoint = glm::vec3(0, 0, 0);

// Set camera's position
Camera camera(glm::vec3(10.f, -2.f, 7.0f));
Physics physics;
int dimension = 64;

unsigned num_obj = 6;

void FrameBufferSizeCallback(GLFWwindow* window, int _width, int _height)
{
	UNREFERENCED_PARAMETER(window);
	glViewport(0, 0, _width, _height);
}

int main(void)
{
	// Initialize the library
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, "Graphics_Physics_TechDemo", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	// Make the window's context current
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	std::vector<Object> main_obj;
	for (unsigned i = 0; i < num_obj; ++i)
	{
		Object objs(O_SPHERE, glm::vec3(-5.f + 3 * i, 0.f, -2.f), glm::vec3(1.f,1.f,1.f), dimension);
		main_obj.push_back(objs);
	}

	//////////////////////////PHYSICS TEST//////////////////
	Object main_obj_texture(O_SPHERE, glm::vec3(0.9f, -2.5f, 2.f), glm::vec3(1.f,1.f,1.f), dimension);
	physics.push_object(&main_obj_texture);

	SoftBodyPhysics plain(O_PLANE, glm::vec3(0, 1.5f, 1.f), glm::vec3(4.f, 1.f, 7.f), dimension);
	physics.push_object(&plain);
	
	Shader pbrshader(GL_FALSE, Shader::S_PBR);
	Shader pbr_texture_shader(GL_FALSE, Shader::S_PBR_TEXTURE);
	Shader equirectangularToCubmapShader(GL_FALSE, Shader::S_EQUIRECTANGULAR);
	Shader irradianceShader(GL_FALSE, Shader::S_IRRADIANCE);
	Shader backgroundShader(GL_FALSE, Shader::S_BACKGROUND);

	pbrshader.Use();
	pbrshader.SetInt("irradianceMap", 0);
	pbrshader.SetVec3("albedo", glm::vec3(0.5f, 0.f, 0.f));
	pbrshader.SetFloat("ao", 1.0f);

	pbr_texture_shader.Use();
	pbr_texture_shader.SetInt("albedoMap", 1);
	pbr_texture_shader.SetInt("normalMap", 2);
	pbr_texture_shader.SetInt("metallicMap", 3);
	pbr_texture_shader.SetInt("roughnessMap", 4);
	pbr_texture_shader.SetInt("aoMap", 5);

	// load PBR material textures
	unsigned int albedo = main_obj_texture.loadTexture("models\\pbr\\rusted_iron\\albedo.png");
	unsigned int normal = main_obj_texture.loadTexture("models\\pbr\\rusted_iron\\normal.png");
	unsigned int metallic = main_obj_texture.loadTexture("models\\pbr\\rusted_iron\\metallic.png");
	unsigned int roughness = main_obj_texture.loadTexture("models\\pbr\\rusted_iron\\roughness.png");
	unsigned int ao = main_obj_texture.loadTexture("models\\pbr\\rusted_iron\\ao.png");

	backgroundShader.Use();
	backgroundShader.SetInt("environmentMap", 0);

	Light light[4];
	light[0].position = glm::vec3(10.f, 10.f, 10.f);
	light[1].position = glm::vec3(-10.f, 10.f, 10.f);
	light[2].position = glm::vec3(-10.f, -10.f, 10.f);
	light[3].position = glm::vec3(10.f, -10.f, 10.f);
	light[0].color    = glm::vec3(300.f, 300.f, 300.f);
	for (int i = 1; i < 4; ++i)
		light[i].color = light[0].color;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// pbr: setup framebuffer
	unsigned int captureFBO;
	unsigned int captureRBO;
	unsigned int envCubemap;
	unsigned int irradianceMap;
	InitFrameBuffer(&equirectangularToCubmapShader, &irradianceShader, captureFBO, captureRBO, envCubemap, irradianceMap);
	InitSkybox(&backgroundShader, &pbrshader, &camera, (float)width, (float)height);

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	bool show_demo_window = false;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		float currFrame = (float)glfwGetTime();
		deltaTime = currFrame - lastFrame;
		lastFrame = currFrame;

		// Input
		ProcessInput(&camera, window, deltaTime);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		{
			ImGui::Begin("GUI interface");
			ImGui::Text("imgui interface");

			ImGui::End();
		}
		//////////////physics update////////////
		if (deltaTime < 1.f)
		{
			physics.update(deltaTime);
			plain.Describe();
		}

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pbr_texture_shader.Use();
		camera.Update(&pbr_texture_shader);
		pbr_texture_shader.SetVec3("camPos", camera.position);

		// bind pre-computed IBL data
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, metallic);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ao);

		main_obj_texture.render_textured(&camera, &pbr_texture_shader, main_obj_texture.position, aspect);
		//main_obj_texture.render_line(&camera, &pbr_texture_shader, main_obj_texture.position, aspect);
		//plain.render_textured(&camera, &pbr_texture_shader, plain.position, aspect);
		plain.render_line(&camera, &pbr_texture_shader, plain.position, aspect);

		// lighting
		for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
		{
			glm::vec3 newPos = light[i].position + glm::vec3(sin(deltaTime * 5.0) * 5.0, 0.0, 0.0);
			newPos = light[i].position;
			pbr_texture_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			pbr_texture_shader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
		}

		pbrshader.Use();
		camera.Update(&pbrshader);
		pbrshader.SetVec3("camPos", camera.position);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

		for (unsigned i = 0; i < num_obj; ++i)
		{
			pbrshader.SetFloat("metallic", (float)i / (float)num_obj);
			pbrshader.SetFloat("roughness", glm::clamp((float)i / (float)num_obj, 0.05f, 1.0f));
			main_obj[i].render_textured(&camera, &pbrshader, main_obj[i].position, aspect);
		}

		// lighting
		for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
		{
			glm::vec3 newPos = light[i].position + glm::vec3(sin(deltaTime * 5.0) * 5.0, 0.0, 0.0);
			newPos = light[i].position;
			pbrshader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			pbrshader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
		}

		// render skybox (render as last to prevent overdraw)
		renderSkybox(&backgroundShader, &camera, envCubemap, irradianceMap);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}