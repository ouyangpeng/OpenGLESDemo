#pragma once
#include <GLES2/gl2.h>
#include "../graphics/Matrix.h"

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1
namespace TRIANGLE3 {
	class NativeTriangle3 {

	public:
		NativeTriangle3();

		~NativeTriangle3();

		void create();

		void change(int width, int height);

		void draw();

	private:
		GLuint mProgram;
		GLuint vboIds[3];
		int mWidth;
		int mHeight;
	};
}


