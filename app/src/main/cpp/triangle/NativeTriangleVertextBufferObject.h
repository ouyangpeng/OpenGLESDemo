#pragma once
#include "../graphics/Matrix.h"
#include "../utils/GLUtils.h"
namespace TRIANGLE_VERTEXT_BUFFER_OBJECT {
    #define VERTEX_POS_SIZE       3 // x, y and z
    #define VERTEX_COLOR_SIZE     4 // r, g, b, and a

    #define VERTEX_POS_INDX       0
    #define VERTEX_COLOR_INDX     1

	class NativeTriangle{

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

		// x-offset uniform location
		GLuint offsetLoc;

		int mWidth;
		int mHeight;

        void DrawPrimitiveWithoutVBOs(GLfloat *vertices, GLint vtxStride,
        		GLint numIndices, GLushort *indices);

        void DrawPrimitiveWithVBOs(GLint numVertices, GLfloat *vtxBuf,
        		GLint vtxStride, GLint numIndices,GLushort *indices);
    };
}


