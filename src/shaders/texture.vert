#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vTex;

out vec2 tex;

uniform mat4 model;

layout(std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main()
{
	gl_Position = projection * view * model * vPos;
	tex = vTex;
}