/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Input.h
Purpose: Input implementation from mouse and keyboard
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Charlie Jung, jungdae.chur
Creation date: 9/6/2018
End Header --------------------------------------------------------*/
#pragma once

#include "GLFW\glfw3.h"
#include "Camera.h"

#if !defined(UNREFERENCED_PARAMETER)
	#define UNREFERENCED_PARAMETER(P) (P)
#endif

void ProcessInput(Camera* camera, GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera->ProcessKeyboard(LOOK_RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera->ProcessKeyboard(LOOK_LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera->ProcessKeyboard(LOOK_UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera->ProcessKeyboard(LOOK_DOWN, deltaTime);
}
void MouseCallback(GLFWwindow* window, Camera* camera, double xpos, double ypos, bool& firstMouse, float& lastX, float& lastY)
{
	UNREFERENCED_PARAMETER(window);
	float f_xpos = float(xpos);
	float f_ypos = float(ypos);

	if (firstMouse)
	{
		lastX = f_xpos;
		lastY = f_ypos;
		firstMouse = false;
	}

	float xoffset = f_xpos - lastX;
	float yoffset = lastY - f_ypos;

	lastX = f_xpos;
	lastY = f_ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, Camera* camera, double xoffset, double yoffset)
{
	UNREFERENCED_PARAMETER(window);
	UNREFERENCED_PARAMETER(xoffset);
	camera->ProcessMouseScroll(float(yoffset));
}
