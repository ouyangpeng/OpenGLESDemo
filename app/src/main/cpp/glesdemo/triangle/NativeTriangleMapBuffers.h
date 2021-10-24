#pragma once
#include "../../utils/GLUtils.h"

namespace TRIANGLE_MapBuffers {
    #define VERTEX_POS_SIZE       3 // x, y and z
    #define VERTEX_COLOR_SIZE     4 // r, g, b, and a

    #define VERTEX_POS_INDX       0
    #define VERTEX_COLOR_INDX     1

	class NativeTriangleMapBuffers {

	public:
		NativeTriangleMapBuffers();

		~NativeTriangleMapBuffers();

		void create();

		void change(int width, int height);

		void draw();

	private:
		GLuint mProgram;
		GLuint vboIds[2];

		int mWidth;
		int mHeight;

        void DrawPrimitiveWithVBOsMapBuffers(GLint numVertices, GLfloat *vtxBuf,
                                             GLint vtxStride, GLint numIndices,
                                             GLushort *indices);
	};
}


