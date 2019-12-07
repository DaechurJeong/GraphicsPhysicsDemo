/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.cpp
Purpose: Create shaders and link with program
Language: MSVC C++
Platform: VS2019, Windows
Project: Graphics_Physics_TechDemo
Author: Charlie Jung, jungdae.chur
Creation date: 9/6/2018
End Header --------------------------------------------------------*/
#include "Shader.h"
#include "glad//glad.h"

#include <vector>
#include <fstream>
#include <sstream>

Shader::~Shader() { glDeleteProgram(m_programId); }

void Shader::CreateShader(const char* vertex_file_path, const char* fragment_file_path,
	const char* geometry_file_path)
{
	bool UseGeometry = false;
	if (geometry_file_path != nullptr)
		UseGeometry = true;
	// vertex shader
	m_vertexId = glCreateShader(GL_VERTEX_SHADER);

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

	if (VertexShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else
	{
		printf("Impossible to open %s.\n", vertex_file_path);
		(void)getchar();
		return;
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);

	if (FragmentShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else
	{
		printf("Impossible to open %s.\n", fragment_file_path);
		(void)getchar();
		return;
	}

	// vertex shader
	char const* vertexSourcePointer = VertexShaderCode.c_str();
	int InfoLogLength;
	glShaderSource(m_vertexId, 1, &vertexSourcePointer, NULL);

	glCompileShader(m_vertexId);

	glGetShaderiv(m_vertexId, GL_COMPILE_STATUS, &m_result);
	glGetShaderiv(m_vertexId, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(static_cast<INT64>(InfoLogLength + 1.0));
		glGetShaderInfoLog(m_vertexId, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
	}

	// geometry shader
	if (UseGeometry)
	{
		std::string GeometryShaderCode;
		std::ifstream GeometryShaderStream(geometry_file_path, std::ios::in);

		if (GeometryShaderStream.is_open())
		{
			std::stringstream sstr;
			sstr << GeometryShaderStream.rdbuf();
			GeometryShaderCode = sstr.str();
			GeometryShaderStream.close();
		}

		m_geometryId = glCreateShader(GL_GEOMETRY_SHADER);

		char const* geometrySourcePointer = GeometryShaderCode.c_str();

		glShaderSource(m_geometryId, 1, &geometrySourcePointer, NULL);

		glCompileShader(m_geometryId);

		glGetShaderiv(m_geometryId, GL_COMPILE_STATUS, &m_result);
		glGetShaderiv(m_geometryId, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0)
		{
			std::vector<char> GeometryShaderErrorMessage(static_cast<INT64>(InfoLogLength + 1.0));
			glGetShaderInfoLog(m_geometryId, InfoLogLength, nullptr, &GeometryShaderErrorMessage[0]);
		}
	}
	// fragment shader
	m_fragmentId = glCreateShader(GL_FRAGMENT_SHADER);

	char const* fragmentSourcePointer = FragmentShaderCode.c_str();

	glShaderSource(m_fragmentId, 1, &fragmentSourcePointer, NULL);

	glCompileShader(m_fragmentId);

	glGetShaderiv(m_fragmentId, GL_COMPILE_STATUS, &m_result);
	glGetShaderiv(m_fragmentId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(static_cast<INT64>(InfoLogLength + 1.0));
		glGetShaderInfoLog(m_fragmentId, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
	}

	// link to program
	m_programId = glCreateProgram();
	if (m_programId == 0)
	{
		printf("Shader cannot get program id from the other.\n");
	}
	else
	{
		glAttachShader(m_programId, m_vertexId);
		if (UseGeometry)
			glAttachShader(m_programId, m_geometryId);
		glAttachShader(m_programId, m_fragmentId);

		glLinkProgram(m_programId);

		glGetProgramiv(m_programId, GL_LINK_STATUS, &m_result);
		glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(static_cast<INT64>(InfoLogLength + 1.0));
			glGetProgramInfoLog(m_programId, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		}
	}
	glDeleteShader(m_vertexId);
	if (UseGeometry)
		glDeleteShader(m_geometryId);
	glDeleteShader(m_fragmentId);
}
void Shader::Use() const
{
	glUseProgram(m_programId);
}
void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_programId, name.c_str()), 1, &value[0]);
}
void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_programId, name.c_str()), value);
}
void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}
void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}