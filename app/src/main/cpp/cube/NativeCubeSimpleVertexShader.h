#pragma once
#include "../utils/GLUtils.h"
#include "../graphics/Matrix.h"

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

namespace TRIANGLE_SIMPLE_VERTEXT_SHADER{
	class NativeTriangle {
	public:
		NativeTriangle();

		~NativeTriangle();

		void create();

		void change(int width, int height);

		void draw();

	private:
		// Handle to a program object
		GLuint mProgram;

		// Uniform locations
		GLint mvpLoc;

		// Vertex daata
		GLfloat * vertices;
		GLuint * indices;

		int numIndices;

		// Rotation angle
		GLfloat angle;

        // MVP matrix
        ESMatrix  mvpMatrix;

		int mWidth;
		int mHeight;

		long mLastTime = 0;

		void update(float deltaTime);

		float getDeltaTime();
	};
}


