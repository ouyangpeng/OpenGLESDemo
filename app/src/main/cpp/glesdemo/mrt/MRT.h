//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_MRT_H
#define OPENGLESDEMO_MRT_H

#include "GLUtils.h"

class MRT {
public:
	MRT();

	~MRT();

	void create();

	void change(int width, int height);

	void draw();

	void shutdown();

private:
	// Handle to a program object
	GLuint programObject;

	// Handle to a framebuffer object
	GLuint fbo;

	// Texture handle
	GLuint colorTexId[4];

	// Texture size
	GLsizei textureWidth;
	GLsizei textureHeight;

	int mWidth;
	int mHeight;

	void initFBO();

	void drawGeometry();

	void blitTextures();
};


#endif //OPENGLESDEMO_MRT_H
