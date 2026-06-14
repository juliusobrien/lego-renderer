#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vCol;

out vec3 colour;
uniform mat4 transformation;

void main()
{
	gl_Position = transformation * vPos;
	colour = vCol;
}