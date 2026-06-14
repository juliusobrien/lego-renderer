#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"

#include "shadow.h"

//light direction variable here
glm::vec3 lightDirection = glm::vec3(0.1f, -0.81f, -0.61f);
glm::vec3 lightPos = glm::vec3(2.0f, 6.0f, 7.0f);
SCamera Camera;
  
float vertices[] =
{
	//back face
	//pos					//col				//normal
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, -1.f,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, -1.f,

	//front face
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 0.f, 1.f,

	//left face
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	-1.f, 0.f, 0.f,

	//right face
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f, -0.5f, -0.5f, 	1.0f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	1.f, 0.f, 0.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	1.f, 0.f, 0.f,

	//bottom face
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	-0.5f, -0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, -1.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, -1.f, 0.f,

	//top face
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 1.f, 0.f,
	0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 1.f, 0.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 1.f, 0.f,
	0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 1.f, 0.f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,	0.f, 1.f, 0.f,
	-0.5f,  0.5f, -0.5f, 	1.0f, 1.0f, 1.0f,	0.f, 1.f, 0.f
};

#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1024
#define HEIGHT 768
#define SH_MAP_WIDTH 2048/2
#define SH_MAP_HEIGHT 2048/2

void drawFloorAndCubes(unsigned int shaderProgram)
{
	glBindVertexArray(VAOs[0]);

	//floor
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0, -3, 0));
	model = glm::scale(model, glm::vec3(100, 0.1, 100));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//cubes
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			for (int k = -1; k < 2; k++)
			{
				glm::mat4 model = glm::mat4(1.f);
				model = glm::translate(model, glm::vec3(float(i * 2), float(j * 2), float(k * 2)));
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}
}

void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		lightDirection = Camera.Front;
		lightPos = Camera.Position;
	}

	float x_offset = 0;
	float y_offset = 0;
	bool cam_changed = false;

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		x_offset = 1;
		y_offset = 0;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		x_offset = -1;
		y_offset = 0;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		x_offset = 0;
		y_offset = -1;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		x_offset = 0;
		y_offset = 1;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cam_dist -= 0.1f;
		cam_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		cam_dist += 0.1f;
		cam_changed = true;
	}

	if (cam_changed)
	{
		MoveAndOrientCamera(Camera, glm::vec3(0.f, 0.f, 0.f), cam_dist, x_offset, y_offset);
	}
}

void SizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void generateDepthMap(unsigned int shadowShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix)
{
	//1. Resize viewport to size of shadow map
	glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);

	//2. Bind depth map buffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);

	//3. Clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	//4. Render scene using model matrices
	glUseProgram(shadowShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	drawFloorAndCubes(shadowShaderProgram);

	//5. Unbind depth map buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderWithShadow(unsigned int renderShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix)
{
	//0. Resize viewport to size of display
	glViewport(0, 0, WIDTH, HEIGHT);

	//1. Clear colour buffer
	static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f };
	glClearBufferfv(GL_COLOR, 0, bgd);
	glClear(GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//2. Activate shader program
	glUseProgram(renderShaderProgram);

	glBindTexture(GL_TEXTURE_2D, shadow.Texture);

	//3. Pass light & camera attributes to uniforms
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightColour"), 255.f/255.f, 250.f/255.f, 244.f/255.f); //Daylight
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);

	//4. Setup view & projection matrices
	glm::mat4 view = glm::mat4(1.f);
	view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	drawFloorAndCubes(renderShaderProgram);
}

int main(int argc, char** argv)
{
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 8);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	gl3wInit();

	int max_samples;
	glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
	printf("Max samples supported = %d\n", max_samples);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	ShadowStruct shadow = setup_shadowmap(SH_MAP_HEIGHT, SH_MAP_WIDTH);

	GLuint program = CompileShader("phong.vert", "phong.frag");
	GLuint shadow_program = CompileShader("shadow.vert", "shadow.frag");

	InitCamera(Camera);
	cam_dist = 5.f;
	MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);

	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		//Setup projected light space matrix
		//Directional light rays are parallel, therefor we use orthogonal projection matrix
		float near_plane = 1.0f, far_plane = 70.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projectedLightSpaceMatrix = lightProjection * lightView;

		glCullFace(GL_FRONT); //Cull front when rendering depth map
		generateDepthMap(shadow_program, shadow, projectedLightSpaceMatrix);
		glCullFace(GL_BACK); //Cull back when rendering scene

		//Call to visualise the depth map
		//saveShadowMapToBitmap(shadow.Texture, SH_MAP_WIDTH, SH_MAP_HEIGHT);

		renderWithShadow(program, shadow, projectedLightSpaceMatrix);

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}