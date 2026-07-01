#pragma once
#include "file.h"

inline GLuint CompileShader(const char* vsFilename, const char* fsFilename)
{
	int success;
	char infoLog[512];

	// Compile Vertex Shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char* vertexShaderSource = read_file(vsFilename);
	if (vertexShaderSource == NULL) {
		fprintf(stderr, "Failed to read shader file: %s\n", vsFilename);
		return 0;
	}
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "Failed vertex shader compiation: %s\n", infoLog);
	}


	// Compile Fragment Shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char* fragmentShaderSource = read_file(fsFilename);
	if (fragmentShaderSource == NULL) {
		fprintf(stderr, "Failed to read shader file: %s\n", fsFilename);
		return 0;
	}
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "Failed fragment shader compiation: %s\n", infoLog);
	}

	// Link Programs
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		fprintf(stderr, "Failed shader program link: %s\n", infoLog);
	}

	// Free memory
	free(vertexShaderSource);
	free(fragmentShaderSource);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Return program
	printf("Loaded shader program: %s & %s\n", vsFilename, fsFilename);
	return program;
}