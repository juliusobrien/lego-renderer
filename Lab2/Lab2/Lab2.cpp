#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "error.h"
#include "file.h"
#include "shader.h"

GLfloat vertices[] =
{
	//pos			//col		
	0.f,  0.5f,		1.f, 0.f, 0.f,
	-0.5f, -0.5f,	0.f, 1.f, 0.f,
	0.5f,  -0.5f,	0.f, 0.f, 1.f
};

#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

float x_offset = 0.f;
float y_offset = 0.f;
float speed = 0.01f;

void SizeCallback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
}

void ProcessKeyboard(GLFWwindow* window)
{
	float velocity;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { 
		velocity = speed * 2;
	} else {
		velocity = speed;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { y_offset += velocity; }
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { y_offset -= velocity; }
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { x_offset += velocity; }
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { x_offset -= velocity; }
}

int main() 
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(640, 480, "A Triangle", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	GLuint program = CompileShader("triangle.vert", "triangle.frag");

	// Creates buffer objects, in VRAM on GPU
	glCreateBuffers(NUM_BUFFERS, Buffers);
	// Allocates size bytes in buffer objects new data store,
	// and copies data from RAM to VRAM data store to init VRAM
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
	// Creates VAOs & stores handles to them in arrays
	glGenVertexArrays(NUM_VAOS, VAOs);
	// Binds VAO array to allow changes
	glBindVertexArray(VAOs[0]);
	// Binds buffer to target
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);

	// Describe attributes
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (5 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (5 * sizeof(float)), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);
		glUniform1f(glGetUniformLocation(program, "x_offset"), x_offset);
		glUniform1f(glGetUniformLocation(program, "y_offset"), y_offset);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();

		ProcessKeyboard(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}