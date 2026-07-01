#version 450 core

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec3 vCol;
layout(location = 2) in vec3 vNor;

uniform mat4 model;
layout(std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out vec3 col;
out vec3 nor;
out vec3 FragPosWorldSpace;

void main()
{
	gl_Position = projection * view * model * vPos;
	col = vCol;
	nor = mat3(transpose(inverse(model))) * vNor;
	FragPosWorldSpace = vec3(model * vPos);
}
