#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vCol;

out vec3 colour;

uniform mat4 model;

layout(std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main()
{
	gl_Position = projection * view * model * vPos;
	colour = vCol;
}