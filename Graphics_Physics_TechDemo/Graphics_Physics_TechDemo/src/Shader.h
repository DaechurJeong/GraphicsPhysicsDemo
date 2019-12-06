/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.h
Purpose: Prototype of shader class
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Charlie Jung, jungdae.chur
Creation date: 9/6/2018
End Header --------------------------------------------------------*/
#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <string>

class Shader {
public:
	enum ShaderType {
		N_NONE,
		S_PBR,
		S_PBR_TEXTURE,
		S_EQUIRECTANGULAR,
		S_IRRADIANCE,
		S_BACKGROUND,
		S_SKYBOX,
		S_BRDF,
		S_PREFILTER,
		S_LIGHT,
	};

	Shader() : m_programId(0), m_vertexId(0), m_fragmentId(0), m_geometryId(0), m_infoLogLength(0), m_result(0){};
	~Shader();

	void CreateShader(const char* vertex_file_path, const char* fragment_file_path,
		const char* geometry_file_path);

	void Use() const;

	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	void SetFloat(const std::string& name, float value) const;
	void SetInt(const std::string& name, int value) const;
	void SetBool(const std::string& name, bool value) const;

private:
	static std::string m_vertexShader, m_fragmentShader, m_geometryShader;
	GLuint m_programId, m_vertexId, m_fragmentId, m_geometryId;
	int m_infoLogLength;
	GLint m_result;

};