#version 400 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

#define NR_LIGHTS 8
uniform vec3 lightPosition;
uniform vec3 lightColor;

void main()
{
	FragColor = vec4(lightColor, 1.f);
}