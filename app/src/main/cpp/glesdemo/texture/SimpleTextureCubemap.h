//
// Created by OuyangPeng on 2021/10/24.
//
#pragma once

#include "../../utils/GLUtils.h"
namespace NAMESPACE_SimpleTextureCubemap {
	class SimpleTextureCubemap {

	public:
		SimpleTextureCubemap();

		~SimpleTextureCubemap();

		void create();

		void change(int width, int height);

		void draw();

	private:
		// Handle to a program object
		GLuint programObject;

		// Sampler location
		GLint samplerLoc;

		// Texture handle
		GLuint textureId;

		// Vertex data
		int      numIndices;
		GLfloat* vertices;
		GLfloat* normals;
		GLuint* indices;

		int mWidth;
		int mHeight;

		static GLuint CreateSimpleTextureCubemap();
	};
}
