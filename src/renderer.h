#pragma once
#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <vector>

#include "shadow.h"
#include "material.h"

struct RenderObj {
	GLuint VAO;
	int vertexCount;
	glm::mat4 model;
	Material material;
	GLuint program;
};

class Renderer {
public:
	void init();
	void stop();

	void clear(static const GLfloat bgd[]);
	void skyboxPass(RenderObj& objects);
	void depthPass(glm::mat4& lightSpaceMatrix, std::vector<RenderObj>& objects);
	void lightingPass(
		const glm::vec3& camPos,
		const glm::vec3& lightDir,
		const glm::vec3& lightPos,
		const glm::vec3& lightColour,
		const glm::mat4& lightSpaceMatrix,
		const std::vector<RenderObj>& objects
	);
private:
	GLint skyboxShader = 0;
	GLint depthShader = 0;
	ShadowStruct shadow = {};
};

