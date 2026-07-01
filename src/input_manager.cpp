#include "input_manager.h"

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void ResizeCallback(GLFWwindow*, int w, int h) {
	glViewport(0, 0, w, h);
}

void CursorCallback(GLFWwindow* window, double xpos, double ypos) {
	InputManager* handler = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

	if (handler->firstMouse) {
		handler->lastX = xpos;
		handler->lastY = ypos;
		handler->firstMouse = false;
	}

	float xoffset = xpos - handler->lastX;
	float yoffset = handler->lastY - ypos; //reversed
	
	handler->lastX = xpos;
	handler->lastY = ypos;

	MoveFlyCamera(*(handler->camera), xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	InputManager* handler = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

	ProcessMouseScroll(*(handler->camera), static_cast<float>(yoffset));
}

void InputManager::init(GLFWwindow* window, SCamera& in, glm::vec3& lightDir, glm::vec3& lightPos) {
	this->camera = &in;
	this->lightDir = &lightDir;
	this->lightPos = &lightPos;
	this->lastX = WINDOW_WIDTH / 2.0f;
	this->lastY = WINDOW_HEIGHT / 2.0f;

	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, ResizeCallback);

	glfwSetCursorPosCallback(window, CursorCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetScrollCallback(window, ScrollCallback);
}

void InputManager::processKeyboard(GLFWwindow* window, float deltaTime) {
	float velocity;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		velocity = camera->MovementSprintSpeed * deltaTime;
	}
	else {
		velocity = camera->MovementSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		*lightDir = camera->Front;
		*lightPos = camera->Position;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->Position += camera->Front * velocity;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->Position -= camera->Front * velocity;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->Position -= camera->Right * velocity;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->Position += camera->Right * velocity;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera->Position += camera->WorldUp * velocity;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		camera->Position -= camera->WorldUp * velocity;
	}
}