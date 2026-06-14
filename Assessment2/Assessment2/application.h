#pragma once
//glfw
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
//local
#include "config.h"
#include "input_manager.h"
#include "camera.h"
#include "renderer.h"
#include "material.h"

class Application {
public:
	void init();
    void run();
	void stop();

private:
    GLFWwindow* window = nullptr;
    // Shader programs
    GLuint program = 0;
	// Textures
	Material materials[NUM_MATERIALS] = {};
	// Buffers and VAOs
    GLuint buffers[NUM_BUFFERS] = {0};
    GLuint VAOs[NUM_VAOS] = {0};
    GLuint uboMatrices = 0;
	// Renderer and scene objects
    Renderer renderer;
    RenderObj skybox;
    std::vector<RenderObj> objects;
	// Input and Camera
    InputManager inputManager;
	SCamera camera;
    glm::vec3 lightDir;
    glm::vec3 lightPos;
	// Timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
    int fps = 0;
    double lastTime = 0.0;
};