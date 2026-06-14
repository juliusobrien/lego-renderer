#pragma once
GLuint CompileShader(const char* vsFilename, const char* fsFilename)
{
	int success;
	char infoLog[512];

	// Compile Vertex Shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char* vertexShaderSource = read_file(vsFilename);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "Vertex Shader Compiation Fail - %s\n", infoLog);
	}


	// Compile Fragment Shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char* fragmentShaderSource = read_file(fsFilename);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "Fragment Shader Compiation Fail - %s\n", infoLog);
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
		fprintf(stderr, "Shader Program Link Fail - %s\n", infoLog);
	}

	// Free memory
	free(vertexShaderSource);
	free(fragmentShaderSource);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}