#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords; 
out vec3 WorldPos;
out vec3 Normal;
	
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	WorldPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}