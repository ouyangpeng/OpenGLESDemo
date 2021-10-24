#pragma once
#include "../graphics/Matrix.h"
#include "../utils/GLUtils.h"
namespace TRIANGLE_VERTEXT_ARRAY_OBJECT {
#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_STRIDE         ( sizeof(GLfloat) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ) )

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

		// VertexBufferObject Ids
		GLuint vboIds[2];

		// VertexArrayObject Id
		GLuint vaoId;

		int mWidth;
		int mHeight;
	};
}


