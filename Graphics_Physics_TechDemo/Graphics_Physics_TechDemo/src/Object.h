/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: Prototype of Object class
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Charlie Jung, jungdae.chur
Creation date: 9/6/2018
End Header --------------------------------------------------------*/
#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "glm/glm.hpp"

#include <vector>
#include <map>
#include <string>

class Camera;
class Shader;

typedef enum ObjShape {
	O_PLANE,
	O_SPHERE,
	O_CUBE,
	O_OBJ,
}ObjectShape;

typedef enum TexType {
	PLASTIC,
	STEEL,
	WOOD,
	RUSTED_IRON,
	FABRIC,
	TORN_FABRIC,
	ALUMINIUM,
	COPPER,
	CONCRETE,
	LEATHER,
	GOLD,
}TextureType;

class Object 
{
private:
	unsigned m_vao = 0, m_vbo = 0, m_ebo = 0, normalBuffer = 0, textureBuffer = 0;
	unsigned m_elementSize = 0;
	int width, height;
	float xMax, xMin, yMax, yMin, zMax, zMin;

public:
	Object(ObjectShape shape, glm::vec3 pos, glm::vec3 scale_, int dim);
	~Object();

	void CreateObject(const char* path, glm::vec3 initial_position, glm::vec3 initial_scale);
	void GenerateBuffers();
	void Describe();
	bool loadOBJ(const char* path, glm::vec3& middlePoint);
	void makeSphere();
	void makePlain();

	void render_objs(Camera* camera, Shader* shader, glm::vec3 pos, float aspect, bool draw_line);
	void render_diff_properties(Camera* camera, Shader* shader, glm::vec3 pos, float aspect);
	void render_lights(Camera* camera, Shader* shader, glm::vec3 pos, float aspect);
	unsigned int loadTexture(const char* path);
	void LoadTGAFile(std::vector<std::string> faces);

	bool loadPPM(const char* path, std::vector<glm::vec3>& values_);
	void SendTextureInfo(Shader* shader, unsigned int& textureBuffer);
	
	unsigned int albedo = 0;
	unsigned int normal = 0;
	unsigned int metallic = 0;
	unsigned int roughness = 0;
	unsigned int ao = 0;

	unsigned right, left, front, back, up, bottom; // indexes
	int nrRows;
	int nrColumns;
	float spacing;

	std::vector<glm::vec3> obj_vertices;
	std::vector<unsigned> obj_indices;
	std::vector<glm::vec2> textureUV;
	std::vector<float> data;
	glm::vec3 position, scale, color;
	std::multimap<int, glm::vec3> faceNormals;
	std::vector<glm::vec3> vertexNormals;
	glm::vec3 middlePoint;
	glm::mat4 m_model;
	unsigned m_textures[6];
	float rotation;
	int dimension;
	ObjectShape m_shape;
	TextureType m_textype;
	glm::vec3 axis;

	bool phy = false;

	std::vector<unsigned> test_indices;
	std::vector<glm::vec2> textureUV_fromIndices;

	//for collision
	glm::vec3 normalVec;
	float d;
};

// helper functions
unsigned int loadTexture_Environment(const char* path);
unsigned int loadTexture_Cubemap();
unsigned int loadTexture_Cubemap(std::vector<std::string> faces);
unsigned int loadTexture_irradianceMap(unsigned int& captureFBO, unsigned int& captureRBO);
unsigned int loadTexture_prefilterMap();
unsigned int loadTexture_LUT(Shader* brdfShader, unsigned captureFBO, unsigned captureRBO);
void simulate_prefilter(Shader* prefilterShader, unsigned prefilterMap, unsigned captureFBO, unsigned captureRBO,
	unsigned envCubemap, glm::mat4* captureViews);

void InitFrameBuffer(Shader* equirectangularToCubmapShader, Shader* irradianceShader, Shader* prefilterShader, Shader* brdfShader,
	unsigned& captureFBO, unsigned& captureRBO,	unsigned& envCubemap, unsigned& irradianceMap, unsigned& prefilterMap, unsigned& brdfLUTTexture, unsigned& hdrTexture);
void UpdateFrameBuffer(Shader* equirectangularToCubmapShader, Shader* irradianceShader, Shader* prefilterShader, Shader* brdfShader,
	unsigned& captureFBO, unsigned& captureRBO, unsigned& envCubemap, unsigned& irradianceMap, unsigned& prefilterMap, unsigned& brdfLUTTexture, unsigned& hdrTexture);
void InitSkybox(Shader* backgroundShader, Shader* pbrshader, Camera* camera, float width, float height);
void renderCube();
void renderQuad();
void renderSkybox(Shader* backgroundShader, Camera* camera, unsigned& envCubemap, unsigned& irradianceMap);
void DeleteBuffers();

#endif
