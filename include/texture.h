#pragma once

#include <iostream>
#include "stb_image.h"

GLuint setup_texture(const char* filename)
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
		printf("Successfully loaded image file: %s\n", filename);
	}

	glGenerateMipmap(GL_TEXTURE_2D); //OpenGL function to generate mipmaps

	// Discard colour data after its loaded to OpenGL
	delete[] pxls;
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// Return Texture object
	return texObject;
}

GLuint setup_mipmaps(const char* filename[], int n)
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
			printf("Successfully loaded image file: %s\n", filename[c]);
		}
		delete[] pxls[c];
	}


	// Discard colour data after its loaded to OpenGL

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// Return Texture object
	return texObject;
}