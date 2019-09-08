/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: Prototype of Model class
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Charlie Jung, jungdae.chur
Creation date: 9/6/2018
End Header --------------------------------------------------------*/
#pragma once
#include "glm/glm.hpp"
#include <vector>
#include <map>
#include <string>

class Camera;
class Shader;

class Object {
private:
	unsigned m_vao, m_vbo, m_ebo, normalBuffer;
	unsigned m_elementSize;
	int width, height;
	float xMax, xMin, yMax, yMin, zMax, zMin;
	float rotation;

	std::vector<glm::vec3> obj_vertices;
	std::vector<unsigned> obj_indices;
	std::vector<glm::vec2> textureUV;
	glm::vec3 position, scale, color;
	std::multimap<int, glm::vec3> faceNormals;
	std::vector<glm::vec3> vertexNormals;
public:
	Object();
	~Object();

	void Rendering(Camera* camera, Shader* shader, float aspect, GLenum mode, glm::vec3 pos);
	void Describe(std::vector<glm::vec3> vertices, std::vector<unsigned> indices, std::vector<glm::vec2> textures);
	bool loadOBJ(const char* path, glm::vec3& middlePoint);
	bool loadPPM(const char* path, std::vector<glm::vec3>& values_);
	void SendTextureInfo(Shader* shader, unsigned int& textureBuffer);
	unsigned int loadTexture(char const* path);

	void LoadTGAFile(std::vector<std::string> faces); // not using
};