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
		S_PH_SHADING,
		S_PBR,
		S_SKYBOX,
	};
	void CreateShader(const char* vertex_file_path, const char* fragment_file_path,
		const char* geometry_file_path);

	void Use() const;

	Shader(GLboolean geometryUse, ShaderType type);
	~Shader();

	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	void SetFloat(const std::string& name, float value) const;
	void SetInt(const std::string& name, int value) const;

private:
	static std::string m_vertexShader, m_fragmentShader, m_geometryShader;
	GLuint m_programId, m_vertexId, m_fragmentId, m_geometryId;
	int m_infoLogLength;
	GLint m_result;

	GLboolean UseGeometry;
	ShaderType m_type;
};