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
#include "Camera.h"
#include "Light.h"
#include "Base.h"
#include "Physics.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Scene.h"

#include <iostream>
#include <vector>

// Global variables
//int width = 1280, height = 800;
int width = 1600, height = 1000;
float deltaTime, lastFrame = 0.f;
float aspect = float(width) / float(height);

// Set camera's position
Camera camera(glm::vec3(10.f, -2.f, 7.0f));

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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
	Scene m_scene(0);
	m_scene.Init(window, &camera);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		m_scene.Update(window, &camera, deltaTime);

		glfwMakeContextCurrent(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	m_scene.DeleteBuffers();
	m_scene.ShutDown();
	m_scene.ImGuiShutdown();
	DeleteBuffers();

	glfwTerminate();
	return 0;
}