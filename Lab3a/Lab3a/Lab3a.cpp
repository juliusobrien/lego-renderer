#define _USE_MATH_DEFINES 

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "error.h"
#include "file.h"
#include "shader.h"


#define NUM_BUFFERS 1
#define NUM_VAOS 1
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void ResizeCallback(GLFWwindow*, int w, int h)
{
	glViewport(0, 0, w, h);
}

#define DEG2RAD(n)	n*(M_PI/180)


//DEFINE YOUR FUNCTION FOR CREATING A CIRCLE HERE
float* CreateCircle(int num_segments, float radius) 
{
	float* verts = (float*)malloc(num_segments * 3 * 3 * sizeof(float));
	float offset = 360.f / num_segments;
	float angle = 0.f;

	int index = 0;
	for (int i = 0; i < num_segments; i++)
	{
		//DEBUG
		//printf("offset: %.2f\t", offset);
		//printf("angle: %.2f\t", angle);
		//printf("angle+offset: %.2f\n", angle+offset);

		if (verts) {
			verts[index++] = 0.f;
			verts[index++] = 0.f;
			verts[index++] = 0.f;

			verts[index++] = sin(DEG2RAD(angle)) * radius;
			verts[index++] = cos(DEG2RAD(angle)) * radius;
			verts[index++] = 0.f;

			verts[index++] = sin(DEG2RAD(angle) + DEG2RAD(offset)) * radius;
			verts[index++] = cos(DEG2RAD(angle) + DEG2RAD(offset)) * radius;
			verts[index++] = 0.f;
		}

		angle += offset;
	}

	return verts;
}


int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 640, "2D modelling", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, ResizeCallback);

	gl3wInit();
	GLuint program = CompileShader("triangle.vert", "triangle.frag");

	//CREATE CIRCLE HERE
	int num_segs = 32;
	float radius = 0.5f;
	float* verts = CreateCircle(num_segs, radius);

	//DEBUG CreateCircle Vertices
	//for (int i = 0; i < num_segs * 3; i++) {
	//	printf("v1: %.2f \tv2: %.2f \tv3: %.2f\n", verts[i], verts[i++], verts[i++]);
	//}

	glCreateBuffers(NUM_BUFFERS, Buffers);
	//COPY VERTICES HERE
	glNamedBufferStorage(Buffers[0], num_segs * 3 * 3 * sizeof(float), verts, 0);
	
	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	//SETUP ATTRIBUTES HERE
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window))
	{
		static const GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, bgd);

		glUseProgram(program);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, num_segs * 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//FREE THE ALLOCATED MEMORY HERE
	free(verts);

	glfwDestroyWindow(window);
	glfwTerminate();
}