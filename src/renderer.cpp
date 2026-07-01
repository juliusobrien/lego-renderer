#include "renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "error.h"
#include "shader.h"
#include "config.h"

void Renderer::init() 
{
	skyboxShader = CompileShader("shaders/skybox.vert", "shaders/skybox.frag");

	shadow = setup_shadowmap(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	depthShader = CompileShader("shaders/shadow.vert", "shaders/shadow.frag");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
}

void Renderer::stop()
{
	glDeleteProgram(skyboxShader);
	glDeleteProgram(depthShader);
	glDeleteFramebuffers(1, &shadow.FBO);
	glDeleteTextures(1, &shadow.Texture);
}

void Renderer::clear(static const GLfloat bgd[])
{
	// Clear buffers
	glClearBufferfv(GL_COLOR, 0, bgd);
	glClear(GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::skyboxPass(RenderObj& skybox)
{
	// Change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);

	// Use skybox shader
	glUseProgram(skyboxShader);

	// Draw skybox
	glBindVertexArray(skybox.VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.material.deffuseTexture);
	glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);
	glDrawArrays(GL_TRIANGLES, 0, skybox.vertexCount);

	// Set depth function back to default
	glDepthFunc(GL_LESS);
}

void Renderer::depthPass(glm::mat4& lightSpaceMatrix, std::vector<RenderObj>& objects) 
{
	glCullFace(GL_FRONT); //Cull front when rendering depth map

	// Resize viewport to size of shadow map
	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

	// Bind shadow FBO and clear depth
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Use depth shader and set light-space matrix
	glUseProgram(depthShader);
	glUniformMatrix4fv(glGetUniformLocation(depthShader, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	// Draw each object
	for (int i = 0; i < objects.size(); i++) {
		glBindVertexArray(objects[i].VAO);
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(objects[i].model));
		glDrawArrays(GL_TRIANGLES, 0, objects[i].vertexCount);
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK); //Cull back when rendering scene

	// Optional: Save shadow map to bitmap for debugging
	//saveShadowMapToBitmap(shadow.Texture, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
}

void Renderer::lightingPass(
	const glm::vec3& camPos,
	const glm::vec3& lightDir,
	const glm::vec3& lightPos,
	const glm::vec3& lightColour,
	const glm::mat4& lightSpaceMatrix,
	const std::vector<RenderObj>& objects) 
{
	// Resize viewport to size of window
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Bind shadow map to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadow.Texture);

	// Draw each object (each can use own shader)
	for (int i = 0; i < objects.size(); i++) {

		GLuint program = objects[i].program;

		// Activate shader program
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, "shadowMap"), 0);
		glUniform1i(glGetUniformLocation(program, "textureMap"), 1);
		glUniform1i(glGetUniformLocation(program, "normalMap"), 2);

		// Set uniforms
		glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDir.x, lightDir.y, lightDir.z);
		glUniform3f(glGetUniformLocation(program, "lightColour"), lightColour.x, lightColour.y, lightColour.z);
		glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(program, "camPos"), camPos.x, camPos.y, camPos.z);
		glUniformMatrix4fv(glGetUniformLocation(program, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		// Bind texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, objects[i].material.deffuseTexture);

		// Bind texture to texture unit 2
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, objects[i].material.normalTexture);

		// Draw Object
		glBindVertexArray(objects[i].VAO);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(objects[i].model));
		glDrawArrays(GL_TRIANGLES, 0, objects[i].vertexCount);
	}

	// Cleanup
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
}
