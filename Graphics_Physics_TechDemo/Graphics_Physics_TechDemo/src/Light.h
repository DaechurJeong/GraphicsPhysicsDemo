/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.h
Purpose: Prototype of Light class
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Charlie Jung, jungdae.chur
Creation date: 9/6/2018
End Header --------------------------------------------------------*/
#pragma once
#include "glm/glm.hpp"
#include "Shader.h"

class Light {
public:
	Light() {};
	~Light() {};

	glm::vec3 position, color;
};

