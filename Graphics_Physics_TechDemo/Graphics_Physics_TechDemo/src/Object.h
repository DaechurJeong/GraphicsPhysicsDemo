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
	unsigned m_vao, m_vbo, m_ebo, normalBuffer, textureBuffer;
	unsigned m_elementSize;
	int width, height;
	float xMax, xMin, yMax, yMin, zMax, zMin;
	float rotation;
	
public:
	Object();
	~Object();

	void CreateObject(const char* path, glm::vec3 initial_position, glm::vec3 initial_scale);
	void Rendering(Camera* camera, Shader* shader, float aspect, GLenum mode, glm::vec3 pos);
	void Describe(std::vector<glm::vec3> vertices, std::vector<unsigned> indices, std::vector<glm::vec2> textures);
	bool loadOBJ(const char* path, glm::vec3& middlePoint);
	void makeSphere();
	unsigned int loadTexture(const char* path);

	bool loadPPM(const char* path, std::vector<glm::vec3>& values_);
	void SendTextureInfo(Shader* shader, unsigned int& textureBuffer);
	void LoadTGAFile(std::vector<std::string> faces); // not using

	std::vector<glm::vec3> obj_vertices;
	std::vector<unsigned> obj_indices;
	std::vector<glm::vec2> textureUV;
	std::vector<float> data;
	glm::vec3 position, scale, color;
	std::multimap<int, glm::vec3> faceNormals;
	std::vector<glm::vec3> vertexNormals;
	glm::vec3 middlePoint;
};