#pragma once
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "camera.h"

class InputManager {
public:
    void init(GLFWwindow* window, SCamera& in, glm::vec3& lightDir, glm::vec3& lightPos);
    void processKeyboard(GLFWwindow* window, float deltaTime);

    SCamera* camera = nullptr;
	glm::vec3* lightDir = nullptr;
    glm::vec3* lightPos = nullptr;

    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;
};
