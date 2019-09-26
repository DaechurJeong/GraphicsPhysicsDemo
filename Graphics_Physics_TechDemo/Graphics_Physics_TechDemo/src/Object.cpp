/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.cpp
Purpose: Create shaders and link with program
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Charlie Jung, jungdae.chur
Creation date: 9/6/2018
End Header --------------------------------------------------------*/

#define PI 3.141592654
#define TWOPI 6.283185308
#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
#include "..\include\stb_image.h"
#include "Object.h"
#include "Shader.h"
#include "Camera.h"

#include <fstream>
#include <iostream>

Object::Object()
	: position(glm::vec3(0,0,0)), scale(glm::vec3(1.f,1.f,1.f)), color(glm::vec3(1.0f, 1.0f, 1.0f)), rotation(0.f),
      xMax(0), xMin(0), yMax(0), yMin(0), zMax(0), zMin(0), width(512), height(512)
{

}
Object::~Object()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &textureBuffer);
	glDeleteBuffers(1, &m_ebo);
}
void Object::CreateObject(const char* path, glm::vec3 initial_position, glm::vec3 initial_scale)
{
	if (!loadOBJ(path, middlePoint))
	{
		std::cout << "Failed to read object_center OBJ file!" << std::endl;
		return;
	}
	//Describe(obj_vertices, obj_indices, textureUV);
}
void Object::GenerateBuffers()
{
	glGenVertexArrays(1, &m_vao);

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &textureBuffer);
	glGenBuffers(1, &m_ebo);
}
void Object::Describe()
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, obj_vertices.size() * sizeof(glm::vec3), &obj_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, textureUV.size() * sizeof(glm::vec2), &textureUV[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexNormals.size() * sizeof(glm::vec3), &vertexNormals[0], GL_STATIC_DRAW);

	const GLsizei vertex_size_stride = 0;
	constexpr GLint three_components_in_vertex_position = 3;
	constexpr GLint two_components_in_vertex_normal = 3;
	constexpr GLint three_components_in_vertex_texture_coordinates = 2;
	constexpr GLenum float_element_type = GL_FLOAT;
	constexpr GLboolean not_fixedpoint = GL_FALSE;
	const void* position_offset_in_vertex = reinterpret_cast<void*>(0);
	const void* normal_offset_in_vertex = reinterpret_cast<void*>(0);
	const void* texture_coordinates_offset_in_vertex = reinterpret_cast<void*>(0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(0, three_components_in_vertex_position, float_element_type, not_fixedpoint, vertex_size_stride, position_offset_in_vertex);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glVertexAttribPointer(1, three_components_in_vertex_texture_coordinates, float_element_type, not_fixedpoint, vertex_size_stride, texture_coordinates_offset_in_vertex);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(2, two_components_in_vertex_normal, float_element_type, not_fixedpoint, vertex_size_stride, normal_offset_in_vertex);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_indices.size() * sizeof(unsigned int), &obj_indices[0], GL_STATIC_DRAW); // indices for texture
	m_elementSize = (unsigned)obj_indices.size();
}
/*void Object::Describe(std::vector<glm::vec3> vertices, std::vector<unsigned> indices, std::vector<glm::vec2> textures)
{
	glGenVertexArrays(1, &m_vao);

	glGenBuffers(1, &m_vbo);

	glGenBuffers(1, &m_ebo);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &textureBuffer);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexNormals.size() * sizeof(glm::vec3), &vertexNormals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(glm::vec2), &textures[0], GL_STATIC_DRAW);

	const GLsizei vertex_size_stride = 0;
	constexpr GLint three_components_in_vertex_position = 3;
	constexpr GLint two_components_in_vertex_normal = 3;
	constexpr GLint three_components_in_vertex_texture_coordinates = 2;
	constexpr GLenum float_element_type = GL_FLOAT;
	constexpr GLboolean not_fixedpoint = GL_FALSE;
	const void* position_offset_in_vertex = reinterpret_cast<void*>(0);
	const void* normal_offset_in_vertex = reinterpret_cast<void*>(0);
	const void* texture_coordinates_offset_in_vertex = reinterpret_cast<void*>(0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(0, three_components_in_vertex_position, float_element_type, not_fixedpoint, vertex_size_stride, position_offset_in_vertex);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(1, two_components_in_vertex_normal, float_element_type, not_fixedpoint, vertex_size_stride, normal_offset_in_vertex);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glVertexAttribPointer(2, three_components_in_vertex_texture_coordinates, float_element_type, not_fixedpoint, vertex_size_stride, texture_coordinates_offset_in_vertex);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	m_elementSize = (unsigned)indices.size() * sizeof(unsigned);
}*/

bool Object::loadOBJ(const char* path, glm::vec3& middlePoint)
{
	float max_x = 0, min_x = 0, max_y = 0, min_y = 0, max_z = 0, min_z = 0, abs_max = 0;
	//isUsingTexture = isUseTexture;
	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return false;
	}
	while (1)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			faceNormals.clear();
			for (unsigned i = 0; i < obj_indices.size(); i += 3)
			{
				glm::vec3 vec1 = obj_vertices[obj_indices[i + 2]] - obj_vertices[obj_indices[i]];
				glm::vec3 vec2 = obj_vertices[obj_indices[i + 1]] - obj_vertices[obj_indices[i]];
				glm::vec3 faceNormal = glm::normalize(glm::cross(vec2, vec1));

				for (int j = 0; j < 3; ++j)
				{
					bool exist = false;
					auto it = faceNormals.equal_range(obj_indices[i + j]);
					for (auto iterator = it.first; iterator != it.second; ++iterator)
					{
						if (iterator->second == faceNormal)
						{
							exist = true;
							break;
						}
					}
					if (!exist)
						faceNormals.insert(std::make_pair(obj_indices[i + j], faceNormal));
				}
			}
			vertexNormals.clear();
			vertexNormals.resize(obj_vertices.size());
			for (auto face_it : faceNormals)
			{
				vertexNormals[face_it.first] += face_it.second;
			}
			for (auto& face_it : vertexNormals)
			{
				face_it = glm::normalize(face_it);
			}

			// Save min & max
			xMax = max_x;
			xMin = min_x;
			yMax = max_y;
			yMin = min_y;
			/* Set position and scale to default [-1, 1] */
			if (abs_max < glm::abs(max_x - min_x))
				abs_max = glm::abs(max_x - min_x);
			if (abs_max < glm::abs(max_y - min_y))
				abs_max = glm::abs(max_y - min_y);
			if (abs_max < glm::abs(max_z - min_z))
				abs_max = glm::abs(max_z - min_z);

			middlePoint = glm::vec3((max_x + min_x) / (2 * abs_max),
				(max_y + min_y) / (2 * abs_max),
				(max_z + min_z) / (2 * abs_max));

			xMax /= (0.5f * abs_max);
			xMin /= (0.5f * abs_max);
			yMax /= (0.5f * abs_max);
			yMin /= (0.5f * abs_max);
			zMax /= (0.5f * abs_max);
			zMin /= (0.5f * abs_max);
			xMax -= (2.f * middlePoint.x);
			xMin -= (2.f * middlePoint.x);
			yMax -= (2.f * middlePoint.y);
			yMin -= (2.f * middlePoint.y);
			zMax -= (2.f * middlePoint.z);
			zMin -= (2.f * middlePoint.z);
			for (unsigned i = 0; i < obj_vertices.size(); ++i)
			{
				obj_vertices[i].x /= (0.5f * abs_max);
				obj_vertices[i].y /= (0.5f * abs_max);
				obj_vertices[i].z /= (0.5f * abs_max);
				obj_vertices[i] -= (2.f * middlePoint);

				glm::vec3 normalized = glm::normalize(obj_vertices[i]);

				float theta = glm::atan(normalized.y / normalized.x);
				glm::vec2 textUV = glm::vec2(theta / TWOPI, (normalized.z + 1) * 0.5f);

				textureUV.push_back(textUV);
			}
			break;
		}
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

			/* Find the min & max data which is in obj */
			if (max_x > vertex.x)
				max_x = vertex.x;
			if (max_y > vertex.y)
				max_y = vertex.y;
			if (max_z > vertex.z)
				max_z = vertex.z;

			if (min_x < vertex.x)
				min_x = vertex.x;
			if (min_y < vertex.y)
				min_y = vertex.y;
			if (min_z < vertex.z)
				min_z = vertex.z;

			obj_vertices.push_back(vertex);
		}

		if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3];
			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			if (matches != 3)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			obj_indices.push_back(vertexIndex[0] - 1);
			obj_indices.push_back(vertexIndex[1] - 1);
			obj_indices.push_back(vertexIndex[2] - 1);
		}
	}
	return true;
}
void Object::makeSphere()
{
	GenerateBuffers();

	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;

	dimension = 64;
	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = static_cast<float>(std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI));
			float yPos = static_cast<float>(std::cos(ySegment * PI));
			float zPos = static_cast<float>(std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI));

			obj_vertices.push_back(glm::vec3(xPos, yPos, zPos));
			textureUV.push_back(glm::vec2(xSegment, ySegment));
			vertexNormals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}
	bool oddRow = false;
	for (int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (int x = 0; x <= X_SEGMENTS; ++x)
			{
				obj_indices.push_back(y * (X_SEGMENTS + 1) + x);
				obj_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				obj_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				obj_indices.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	Describe();
}
void Object::makePlain()
{
	GenerateBuffers();

	/////original
	//const unsigned int X_SEGMENTS = 64;
	//const unsigned int Y_SEGMENTS = 64;

	///for physics test
	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	dimension = 64;

	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = xSegment;
			float yPos = 0;
			float zPos = ySegment;

			obj_vertices.push_back(glm::vec3(xPos, yPos, zPos));
			textureUV.push_back(glm::vec2(xSegment, ySegment));
			vertexNormals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}
	bool oddRow = false;
	for (int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (int x = 0; x <= X_SEGMENTS; ++x)
			{
				obj_indices.push_back(y * (X_SEGMENTS + 1) + x);
				obj_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				obj_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				obj_indices.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	Describe();
}
void Object::render_textured(Camera* camera, Shader* shader, glm::vec3 pos, float aspect)
{
	const static glm::vec3 up(0, 1, 0);

	glm::mat4 identity_translate(1.0);
	glm::mat4 identity_scale(1.0);
	glm::mat4 identity_rotation(1.0);

	glm::mat4 model = glm::translate(identity_translate, pos) * glm::scale(identity_scale, scale) * glm::rotate(identity_rotation, rotation, up);
	glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), aspect, 0.1f, 100.0f); // zoom = fov;
	glm::mat4 view = camera->GetViewMatrix();

	shader->SetMat4("projection", projection);
	shader->SetMat4("model", model);
	shader->SetMat4("view", view);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLE_STRIP, m_elementSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void Object::render_line(Camera* camera, Shader* shader, glm::vec3 pos, float aspect)
{
	const static glm::vec3 up(0, 1, 0);

	glm::mat4 identity_translate(1.0);
	glm::mat4 identity_scale(1.0);
	glm::mat4 identity_rotation(1.0);

	glm::mat4 model = glm::translate(identity_translate, pos) * glm::scale(identity_scale, scale) * glm::rotate(identity_rotation, rotation, up);
	glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), aspect, 0.1f, 100.0f); // zoom = fov;
	glm::mat4 view = camera->GetViewMatrix();

	shader->SetMat4("projection", projection);
	shader->SetMat4("model", model);
	shader->SetMat4("view", view);

	glBindVertexArray(m_vao);
	glDrawElements(GL_LINE_STRIP, m_elementSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
unsigned int Object::loadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glActiveTexture(GL_TEXTURE1 + textureID - 1);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadTexture_Environment(const char* path)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(path, &width, &height, &nrComponents, 0);
	unsigned int hdrTexture = 0;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}
	return hdrTexture;
}
unsigned int loadTexture_Cubemap()
{
	unsigned int envCubemap = 0;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return envCubemap;
}
unsigned int loadTexture_irradianceMap(unsigned int& captureFBO, unsigned int& captureRBO)
{
	unsigned int irradianceMap = 0;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	return irradianceMap;
}