#pragma once
#include <iostream>

#include <glm/glm.hpp>

#include "config.h"

struct SCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;
	float Zoom;

	float MovementSpeed;
	float MovementSprintSpeed;
	float MouseSensitivity;
};

inline void InitCamera(SCamera& in) {
	in.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	in.Position = glm::vec3(0.0f, 0.0f, 0.0f);
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = -90.f;
	in.Pitch = 0.f;
	in.Zoom = CAMERA_DEFAULT_FOV;

	in.MovementSpeed = CAMERA_MOVEMENT_SPEED;
	in.MovementSprintSpeed = CAMERA_MOVEMENT_SPRINT_SPEED;
	in.MouseSensitivity = CAMERA_MOUSE_SENSITIVITY;
}

inline void ProcessMouseScroll(SCamera& in, float yoffset) {
	in.Zoom -= (float)yoffset;
	if (in.Zoom < CAMERA_MIN_FOV) { in.Zoom = CAMERA_MIN_FOV; }
	if (in.Zoom > CAMERA_MAX_FOV) { in.Zoom = CAMERA_MAX_FOV; }
}

inline void MoveFlyCamera(SCamera& in, float xoffset, float yoffset)
{
	// Update yaw and pitch from mouse movement
	in.Yaw += xoffset * in.MouseSensitivity;
	in.Pitch += yoffset * in.MouseSensitivity;

	// Constraint
	if (in.Pitch > 89.9f) in.Pitch = 89.9f;
	if (in.Pitch < -89.9f) in.Pitch = -89.9f;

	// Recalculate Front vector
	in.Front = glm::normalize(glm::vec3(
		cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch)),
		sin(glm::radians(in.Pitch)),
		sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch))
	));
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));
}

inline void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset)
{	
	in.Yaw -= xoffset * in.MovementSpeed;
	in.Pitch -= yoffset * in.MovementSpeed;

	if (in.Pitch > 89.9f) in.Pitch = 89.9f;
	if (in.Pitch < -89.9f) in.Pitch = -89.9f;

	in.Position = glm::vec3(
		cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch)),
		sin(glm::radians(in.Pitch)),
		sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch))
	);

	in.Position *= distance;

	in.Front = glm::normalize(target - in.Position);
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));
}