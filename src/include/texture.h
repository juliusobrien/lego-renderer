#pragma once
#include <iostream>
#include <GL/gl3w.h>
#include "stb_image.h"

static inline GLuint setup_texture(const char* filename)
{
	// Enable textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	// Generate OpenGL Texture object
	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	// Setup texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				// Bi-linear Filtering = GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // trilinear filtering

	// Load image file
	int w, h, chan;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* pxls = stbi_load(filename, &w, &h, &chan, 0);
	if (pxls) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pxls);
		printf("Loaded image file: %s\n", filename);
	} else {
		fprintf(stderr, "Failed to read image file: %s\n", filename);
	}

	//OpenGL function to generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D); 

	// Discard colour data after its loaded to OpenGL
	stbi_image_free(pxls);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// Return Texture object
	return texObject;
}

static inline GLuint setup_mipmaps(const char* filename[], int n)
{
	// Enable textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	// Generate OpenGL Texture object
	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);

	// Setup texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				// Bi-linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // trilinear filtering

	// Load image files
	int w[16], h[16], chan[16];
	unsigned char* pxls[16];
	stbi_set_flip_vertically_on_load(true);

	for (int c = 0; c < n; c++) {
		pxls[c] = stbi_load(filename[c], &w[c], &h[c], &chan[c], 0);
		if (pxls[c]) {
			glTexImage2D(GL_TEXTURE_2D, c, GL_RGB, w[c], h[c], 0, GL_RGB, GL_UNSIGNED_BYTE, pxls[c]);
			printf("Loaded image file: %s\n", filename[c]);
			stbi_image_free(pxls[c]);
		} else {
			fprintf(stderr, "Failed to read image file: %s\n", filename[c]);
			stbi_image_free(pxls[c]);
		}
	}

	// Discard colour data after its loaded to OpenGL

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// Return Texture object
	return texObject;
}

static inline GLuint setup_cubemap(const char* filename[], int n)
{
	// Enable textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	// Generate OpenGL Texture object
	GLuint texObject;
	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texObject);

	// Setup texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load image files
	int w, h, chan;
	unsigned char* pxls[6];
	stbi_set_flip_vertically_on_load(false);
	for (int c = 0; c < n; c++) {
		pxls[c] = stbi_load(filename[c], &w, &h, &chan, 0);
		if (pxls[c]) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + c, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pxls[c]);
			printf("Loaded image file: %s\n", filename[c]);
			stbi_image_free(pxls[c]);
		}
		else {
			fprintf(stderr, "Failed to read image file: %s\n", filename[c]);
			stbi_image_free(pxls[c]);
		}
	}

	// Discard colour data after its loaded to OpenGL
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// Return Texture object
	return texObject;
}
