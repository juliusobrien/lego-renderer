#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"
#include "obj_parse.h"

glm::vec3 lightDirection = glm::vec3(0.1f, -0.81f, -0.61f);
glm::vec3 lightPos = glm::vec3(2.0f, 6.0f, 7.0f);
SCamera Camera;

#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1024
#define HEIGHT 768

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

int main(int argc, char** argv)
{
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	gl3wInit();

	int max_samples;
	glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
	printf("Max samples supported = %d\n", max_samples);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	std::vector<float> obj_vertices;
	//const std::string MODEL_PATH = "objs/cornell2/cornell-box.obj";
	//const std::string MODEL_PATH = "objs/koenigsegg/koenigsegg-agera.obj";
	const std::string MODEL_PATH = "objs/suzanne/suzanne.obj";
	
	obj_parse(MODEL_PATH.c_str(), &obj_vertices);

	GLuint program = CompileShader("phong.vert", "phong.frag");

	InitCamera(Camera);
	cam_dist = 5.f;
	MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);

	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], obj_vertices.size() * sizeof(float), obj_vertices.data(), 0);
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
		static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUseProgram(program);

		glUniform3f(glGetUniformLocation(program, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform3f(glGetUniformLocation(program, "lightColour"), 1.f, 1.f, 1.f);
		glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(program, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);

		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(Camera.Position, Camera.Position + Camera.Front, Camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100.f);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAOs[0]);

		//model
		glm::mat4 model = glm::mat4(1.f);
	    model = glm::translate(model, glm::vec3(0, 0, 0));
		//model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, obj_vertices.size());

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}