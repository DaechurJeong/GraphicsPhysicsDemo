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
#include "Object.h"
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
Camera camera(glm::vec3(0.0f, .7f, 4.0f));

void FrameBufferSizeCallback(GLFWwindow* window, int _width, int _height)
{
	UNREFERENCED_PARAMETER(window);
	glViewport(0, 0, _width, _height);
}
// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
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

	// Make the window's context current
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Object main_obj;
	main_obj.CreateObject("models\\sphere_mid_poly.obj", glm::vec3(0, 0, 0), glm::vec3(1.f, 1.f, 1.f));
	
	int nrRows = 5;
	int nrColumns = 5;
	float spacing = 2.f;

	Shader shader(GL_FALSE, Shader::S_PBR);

	shader.Use();
	shader.SetInt("albedoMap", 0);
	shader.SetInt("normalMap", 1);
	shader.SetInt("metallicMap", 2);
	shader.SetInt("roughnessMap", 3);
	shader.SetInt("aoMap", 4);

	// load PBR material textures
	unsigned int albedo    = main_obj.loadTexture("models\\pbr\\rusted_iron\\albedo.png");
	unsigned int normal    = main_obj.loadTexture("models\\pbr\\rusted_iron\\normal.png");
	unsigned int metallic  = main_obj.loadTexture("models\\pbr\\rusted_iron\\metallic.png");
	unsigned int roughness = main_obj.loadTexture("models\\pbr\\rusted_iron\\roughness.png");
	unsigned int ao        = main_obj.loadTexture("models\\pbr\\rusted_iron\\ao.png");

	Light light[4];
	light[0].position = glm::vec3(8.f, 5.f, 6.f);
	light[0].color    = glm::vec3(100.f, 100.f, 100.f);
	for (int i = 1; i < 4; ++i)
	{
		light[i].position = light[0].position;
		light[i].color = light[0].color;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)width / (float)height, 0.1f, 100.0f);
	shader.Use();
	shader.SetMat4("projection", projection);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		float currFrame = (float)glfwGetTime();
		deltaTime = currFrame - lastFrame;
		lastFrame = currFrame;

		// Input
		ProcessInput(&camera, window, deltaTime);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		camera.Update(&shader);
		shader.SetVec3("camPos", camera.position);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao);

		// render rows*column number of spheres, for now, only one
		main_obj.Rendering(&camera, &shader, aspect, GL_TRIANGLE_STRIP, main_obj.position);

		// lighting
		for (unsigned int i = 0; i < sizeof(light) / sizeof(light[0]); ++i)
		{
			glm::vec3 newPos = light[i].position + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			newPos = light[i].position;
			shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			shader.SetVec3("lightColors[" + std::to_string(i) + "]", light[i].color);
			//main_obj.position = newPos;
			//main_obj.Rendering(&camera, &shader, aspect, GL_TRIANGLE_STRIP, main_obj.position);
		}

		glfwMakeContextCurrent(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}