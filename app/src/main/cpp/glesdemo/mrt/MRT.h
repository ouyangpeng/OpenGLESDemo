//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_MRT_H
#define OPENGLESDEMO_MRT_H

#include "GLUtils.h"

class MRT : public BaseGLSample{
public:
	MRT();

	virtual ~MRT();

	virtual void create();

	virtual void change(int width, int height);

	virtual void draw();

	virtual void shutdown();

private:
	// Handle to a framebuffer object
	GLuint fbo;

	// Texture handle
	GLuint colorTexId[4];

	// Texture size
	GLsizei textureWidth;
	GLsizei textureHeight;

	void initFBO();

	void drawGeometry();

	void blitTextures();
};


#endif //OPENGLESDEMO_MRT_H
