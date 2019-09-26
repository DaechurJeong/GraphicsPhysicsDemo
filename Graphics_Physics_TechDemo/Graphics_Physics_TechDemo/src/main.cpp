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

unsigned num_obj = 6;

void FrameBufferSizeCallback(GLFWwindow* window, int _width, int _height)
{
	UNREFERENCED_PARAMETER(window);
	glViewport(0, 0, _width, _height);
}
// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
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

	Object main_obj[6];
	for (unsigned i = 0; i < num_obj; ++i)
	{
		main_obj[i].makeSphere();
		main_obj[i].position = glm::vec3(-5.f + 3 * i, 0.f, -2.f);
	}



	//////////////////////////PHYSICS TEST//////////////////
	Object main_obj_texture;
	main_obj_texture.makeSphere();
	main_obj_texture.position = glm::vec3(0.9f, -2.5f, 2.f);
	//main_obj.CreateObject("models\\sphere_mid_poly.obj", glm::vec3(0, 0, 0), glm::vec3(1.f, 1.f, 1.f));
	physics.push_object(&main_obj_texture);

	SoftBodyPhysics plain;
	plain.makePlain();
	plain.position = glm::vec3(0, 1.5f, 1.f);
	plain.scale = glm::vec3(4.f, 1.f, 7.f);
	plain.Init();
	
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
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// pbr: load the HDR environment map
	unsigned int hdrTexture = loadTexture_Environment("models\\newport_loft.hdr");

	// pbr: setup cubemap to render to and attach to framebuffer
	unsigned int envCubemap = loadTexture_Cubemap();

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	equirectangularToCubmapShader.Use();
	equirectangularToCubmapShader.SetInt("equirectangularMap", 0);
	equirectangularToCubmapShader.SetMat4("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubmapShader.SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	unsigned int irradianceMap = loadTexture_irradianceMap(captureFBO, captureRBO);

	irradianceShader.Use();
	irradianceShader.SetInt("environmentMap", 0);
	irradianceShader.SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// initialize static shader uniforms before rendering
	glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)width / (float)height, 0.1f, 100.0f);
	pbrshader.Use();
	pbrshader.SetMat4("projection", projection);
	backgroundShader.Use();
	backgroundShader.SetMat4("projection", projection);

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		float currFrame = (float)glfwGetTime();
		deltaTime = currFrame - lastFrame;
		lastFrame = currFrame;

		// Input
		ProcessInput(&camera, window, deltaTime);

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
		backgroundShader.Use();
		backgroundShader.SetMat4("view", camera.GetViewMatrix());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
		renderCube();

		glfwMakeContextCurrent(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}