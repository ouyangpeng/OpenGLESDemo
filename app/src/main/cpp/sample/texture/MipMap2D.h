//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_MIPMAP2D_H
#define OPENGLESDEMO_MIPMAP2D_H

#include <BaseGLSample.h>

class MipMap2D : public BaseGLSample{
public:
	MipMap2D();

	virtual ~MipMap2D();

	virtual void create();

	virtual void change(int width, int height);

	virtual void draw();

	virtual void shutdown();

private:

	// Sampler location
	GLint samplerLoc;

	// Offset location
	GLint offsetLoc;

	// Texture handle
	GLuint textureId;

	GLuint CreateMipMappedTexture2D();

	GLboolean GenMipMap2D(const GLubyte* src, GLubyte** dst, int srcWidth, int srcHeight, int* dstWidth, int* dstHeight);

	GLubyte* GenCheckImage(int width, int height, int checkSize);
};


#endif //OPENGLESDEMO_MIPMAP2D_H
