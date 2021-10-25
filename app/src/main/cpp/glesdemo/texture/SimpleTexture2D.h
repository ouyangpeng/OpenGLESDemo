//
// Created by OuyangPeng on 2021/10/24.
//

#ifndef OPENGLESDEMO_SIMPLETEXTURE2D_H
#define OPENGLESDEMO_SIMPLETEXTURE2D_H

#include "../../utils/GLUtils.h"
namespace NAMESPACE_SimpleTexture2D {
	class SimpleTexture2D {

	public:
		SimpleTexture2D();

		~SimpleTexture2D();

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

		int mWidth;
		int mHeight;

		GLuint CreateSimpleTexture2D();
	};
}
#endif //OPENGLESDEMO_SIMPLETEXTURE2D_H
