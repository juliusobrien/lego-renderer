#include "application.h"
#include <iostream>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//local
#include "error.h"
#include "shader.h"
#include "texture.h"
#include "primitives.h"

// Initialisation
void Application::init() {
	// Create Window and OpenGL Context
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, WINDOW_MSAA_SAMPLES);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (window == nullptr) {
		fprintf(stderr, "Failed to create window\n");
		return;
	}
	glfwMakeContextCurrent(window);
	inputManager.init(window, camera, lightDir, lightPos); // Setup Callbacks
	gl3wInit();
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	// Anti-aliasing
	int max_samples;
	glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
	if (WINDOW_MSAA_SAMPLES > max_samples) {
		fprintf(stderr, "MSAA samples exceeds maximum supported\n");
		return;
	}
	printf("Max samples: %d\n", max_samples);
	printf("Using samples: %d\n", WINDOW_MSAA_SAMPLES);

	// Load Resources
	program = CompileShader("shaders/directional_phong_shadow.vert", "shaders/directional_phong_shadow.frag");

	// Stone
	materials[0].deffuseTexture = setup_texture("assets/textures/stone.bmp");
	materials[0].normalTexture = setup_texture("assets/textures/stone_norm.bmp");
	// Wood
	materials[1].deffuseTexture = setup_texture("assets/textures/wood.bmp");
	materials[1].normalTexture = setup_texture("assets/textures/wood_norm.bmp");
	// Grass
	materials[2].deffuseTexture = setup_texture("assets/textures/grass.bmp");
	materials[2].normalTexture = setup_texture("assets/textures/grass_norm.bmp");
	// Fabric
	materials[3].deffuseTexture = setup_texture("assets/textures/fabric.bmp");
	materials[3].normalTexture = setup_texture("assets/textures/fabric_norm.bmp");
	// Concrete
	materials[4].deffuseTexture = setup_texture("assets/textures/concrete.bmp");
	materials[4].normalTexture = setup_texture("assets/textures/concrete_norm.bmp");
	// Stone Tile
	materials[5].deffuseTexture = setup_texture("assets/textures/stone_tile.bmp");
	materials[5].normalTexture = setup_texture("assets/textures/stone_tile_norm.bmp");
	// Corrugated Metal
	materials[6].deffuseTexture = setup_texture("assets/textures/corrugated_metal.bmp");
	materials[6].normalTexture = setup_texture("assets/textures/corrugated_metal_norm.bmp");

	// Setup Camera
	InitCamera(camera);
	camera.Position = glm::vec3(0.f, 1.f, 2.f);
	lightDir = glm::vec3(0.1f, -0.81f, -0.61f);
	lightPos = glm::vec3(2.0f, 6.0f, 7.0f);

	// Setup OpenGL State, Buffers, VAOs, UBOs
	glCreateBuffers(NUM_BUFFERS, buffers);
	glNamedBufferStorage(buffers[0], sizeof(textured_cube_vertices), textured_cube_vertices, 0);
	glNamedBufferStorage(buffers[1], sizeof(textured_plane_vertices), textured_plane_vertices, 0);
	glGenVertexArrays(NUM_VAOS, VAOs);

	// Cube VAO
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0); //Pos
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (8 * sizeof(float)), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1); //Tex
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (8 * sizeof(float)), (void*)(sizeof(float) * 5));
	glEnableVertexAttribArray(2); //Norm

	// Plane VAO
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	const int stride = 11 * sizeof(float);
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	//texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	//normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 5));
	glEnableVertexAttribArray(2);
	//tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 8));
	glEnableVertexAttribArray(3);

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// UBOs
	glCreateBuffers(1, &uboMatrices);
	glNamedBufferData(uboMatrices, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);

	// Renderer
	renderer.init();

	// Objects
	RenderObj cube = {};
	cube.VAO = VAOs[0];
	cube.vertexCount = 36;
	cube.material = materials[0];
	cube.program = program;
	//objects.push_back(cube);

	RenderObj cube2 = {};
	cube2.VAO = VAOs[0];
	cube2.vertexCount = 36;
	cube2.material = materials[1];
	cube2.program = program;
	//objects.push_back(cube2);

	RenderObj cube3 = {};
	cube3.VAO = VAOs[0];
	cube3.vertexCount = 36;
	cube3.material = materials[2];
	cube3.program = program;
	//objects.push_back(cube3);

	RenderObj cube4 = {};
	cube4.VAO = VAOs[0];
	cube4.vertexCount = 36;
	cube4.material = materials[3];
	cube4.program = program;
	//objects.push_back(cube4);

	RenderObj plane = {};
	plane.VAO = VAOs[1];
	plane.vertexCount = 6;
	plane.material = materials[5];
	plane.program = program;
	objects.push_back(plane);

	// Setup skybox cubemap object
	const char* files[6] = {
		"assets/cubemaps/daylight_right.bmp",
		"assets/cubemaps/daylight_left.bmp",
		"assets/cubemaps/daylight_top.bmp",
		"assets/cubemaps/daylight_bottom.bmp",
		"assets/cubemaps/daylight_front.bmp",
		"assets/cubemaps/daylight_back.bmp"
	};
	skybox.VAO = skyboxVAO;
	skybox.vertexCount = 36;
	skybox.material.deffuseTexture = setup_cubemap(files, 6);
}

// Main Loop
void Application::run() {
	while (!glfwWindowShouldClose(window)) {

		// Delta Time and FPS
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		fps++;
		if (currentTime - lastTime >= 1.0) {
			//printf("%f ms/frame\n", 1000.0 / double(fps));
			std::clog << "\rms/frame: " << 1000.0 / double(fps) << std::flush;
			fps = 0;
			lastTime = currentTime;
		}

		// Update UBOs view and projection matrices
		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
		glNamedBufferSubData(uboMatrices, 0, sizeof(glm::mat4), glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, .01f, 25.f);
		glNamedBufferSubData(uboMatrices, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

		// Update Object model matrices
		{
			//glm::mat4 model1 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 0.f));
			//objects[0].model = glm::rotate(model1, (float)glfwGetTime() / 2.f, glm::vec3(1.f, 1.f, 0.f));

			//glm::mat4 model2 = glm::translate(glm::mat4(1.f), glm::vec3(2.f, 1.f, 2.f));
			//objects[1].model = glm::rotate(model2, -(float)glfwGetTime() / 2.f, glm::vec3(0.f, 1.f, 1.f));

			//glm::mat4 model3 = glm::translate(glm::mat4(1.f), glm::vec3(-2.f, 1.f, -2.f));
			//objects[2].model = glm::rotate(model3, (float)glfwGetTime() / 2.f, glm::vec3(1.f, 0.f, 1.f));

			//glm::mat4 model4 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 3.0f, 2.f));
			//model4 = glm::scale(model4, glm::vec3(0.5f));
			//objects[3].model = glm::rotate(model4, (float)glfwGetTime() / 2.f, glm::vec3(1.f, 1.f, 1.f));

			glm::mat4 model5 = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(-1.f, 0.f, 0.f));
			model5 = glm::rotate(model5, (float)glfwGetTime() / 2.f, glm::vec3(1.f, 1.f, 1.f));
			objects[0].model = glm::scale(model5, glm::vec3(1.f));
		}

		// Clear
		GLfloat bgd[] = { 1.f, 1.f, 1.f, 1.f };
		renderer.clear(bgd);

		// Shadow Pass
		float near_plane = 1.0f, far_plane = 70.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		renderer.depthPass(lightSpaceMatrix, objects);

		// Light Pass
		renderer.lightingPass(camera.Position, lightDir, lightPos, glm::vec3(1.f), lightSpaceMatrix, objects);

		// Skybox Pass
		renderer.skyboxPass(skybox);

		// Polling and swap
		inputManager.processKeyboard(window, deltaTime);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}

// Shutdown
void Application::stop() {
	glDeleteProgram(program);
	for (int i = 0; i < NUM_MATERIALS; i++) {
		glDeleteTextures(1, &materials[i].deffuseTexture);
		glDeleteTextures(1, &materials[i].normalTexture);
	}
	glDeleteBuffers(NUM_BUFFERS, buffers);
	glDeleteVertexArrays(NUM_VAOS, VAOs);
	glDeleteBuffers(1, &uboMatrices);

	glfwDestroyWindow(window);
	glfwTerminate();
}