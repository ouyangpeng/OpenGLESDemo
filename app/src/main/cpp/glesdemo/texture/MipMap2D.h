//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_MIPMAP2D_H
#define OPENGLESDEMO_MIPMAP2D_H

#include "../../utils/GLUtils.h"

class MipMap2D {
public:
	MipMap2D();

	~MipMap2D();

	void create();

	void change(int width, int height);

	void draw();

	void shutdown();

private:
	// Handle to a program object
	GLuint programObject;

	// Sampler location
	GLint samplerLoc;

	// Offset location
	GLint offsetLoc;

	// Texture handle
	GLuint textureId;

	int mWidth;
	int mHeight;

	GLuint CreateMipMappedTexture2D();

	GLboolean GenMipMap2D(const GLubyte* src, GLubyte** dst, int srcWidth, int srcHeight, int* dstWidth, int* dstHeight);

	GLubyte* GenCheckImage(int width, int height, int checkSize);
};


#endif //OPENGLESDEMO_MIPMAP2D_H
