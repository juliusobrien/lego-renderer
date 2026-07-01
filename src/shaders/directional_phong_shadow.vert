#version 450 core

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec2 vTex;
layout(location = 2) in vec3 vNor;
layout(location = 3) in vec3 vTangent;

uniform mat4 model;
layout(std140) uniform Matrices {
	uniform mat4 view;
	uniform mat4 projection;
};

uniform mat4 projectedLightSpaceMatrix;

out vec2 tex;
out vec3 nor;
out vec3 FragPosWorldSpace;
out vec4 FragPosProjectedLightSpace;
out mat3 TBN;

void main()
{
	gl_Position = projection * view * model * vPos;
	tex = vTex;
	nor = vNor;

	// World-space normal and tangent (use transpose inverse for non-uniform scale)
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 N = normalize(normalMatrix * vNor);
    vec3 T = normalize(normalMatrix * vTangent);
    // Re-orthogonalize T
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);   // bitangent (handedness absorbed if your tangents are consistently oriented)
    TBN = mat3(T, B, N);

	FragPosWorldSpace = vec3(model * vPos);
	FragPosProjectedLightSpace = projectedLightSpaceMatrix * model * vPos;
}