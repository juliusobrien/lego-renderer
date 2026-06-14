#pragma once

#include "bitmap.h"

struct ShadowStruct
{
	unsigned int FBO;
	unsigned int Texture;
};

ShadowStruct setup_shadowmap(int w, int h)
{
	ShadowStruct shadow;

	glGenFramebuffers(1, &shadow.FBO);				//Create 1 framebuffer and store in struct member
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);	//Bind frame buffer object
	glGenTextures(1, &shadow.Texture);				//Generate 1 texture and store in struct member
	glBindTexture(GL_TEXTURE_2D, shadow.Texture);	//Bind texture
	//Specifies a 2D texture image, used for the depth map.
	//target,
	//level (0 no mipmaps for shadow maps),
	//internalformat is GL_DEPTH_COMPONENT Only using depth component,
	//width height specify the size of the shadow map,
	//border 0,
	//format GL_DEPTH_COMPONENT,
	//type GL_FLOAT depth values from zbuffer are floats,
	//data is NULL.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//Texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//Attach texture to a framebuffer object
	//target must be GL_FRAMEBUFFER,
	//attachment should be GL_DEPTH_ATTACHMENT so values from depth buffer are stored in texture, 
	//textarget should be GL_TEXTURE_2D, 
	//texture object to be attached, use the structure member,
	//level must be 0.
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow.Texture, 0);

	glDrawBuffer(GL_NONE);	//set buf to GL_NONE so no color buffers are written during render pass
	glReadBuffer(GL_NONE);	//set mode to GL_NONE so no color buffers are read during render pass.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return shadow;
}

void saveShadowMapToBitmap(unsigned int Texture, int w, int h)
{
	float* pixelBuffer = (float*)malloc(sizeof(float) * w * h);// [] ;
	glBindTexture(GL_TEXTURE_2D, Texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixelBuffer);

	char* charBuffer = (char*)malloc(sizeof(unsigned char) * w * h * 3);

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			charBuffer[(y * w * 3) + (x * 3) + 0] = pixelBuffer[(y * w) + x] * 255;
			charBuffer[(y * w * 3) + (x * 3) + 1] = pixelBuffer[(y * w) + x] * 255;
			charBuffer[(y * w * 3) + (x * 3) + 2] = pixelBuffer[(y * w) + x] * 255;
		}

	}

	BITMAPINFOHEADER infoHdr;
	infoHdr.biSize = 40;
	infoHdr.biWidth = w;
	infoHdr.biHeight = h;
	infoHdr.biPlanes = 1;
	infoHdr.biBitCount = 24;
	infoHdr.biCompression = 0;
	infoHdr.biSizeImage = sizeof(unsigned char) * w * h * 3;
	infoHdr.biXPelsPerMeter = 0;
	infoHdr.biYPelsPerMeter = 0;
	infoHdr.biClrUsed = 0;
	infoHdr.biClrImportant = 0;

	BITMAPFILEHEADER fileHdr;
	fileHdr.bfType = 19778;
	fileHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(unsigned char) * w * h * 3);
	fileHdr.bfReserved1 = 0;
	fileHdr.bfReserved2 = 0;
	fileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	savebitmap("shadowMap.bmp", (unsigned char*)charBuffer, &infoHdr, &fileHdr);

	free(charBuffer);
	free(pixelBuffer);
}