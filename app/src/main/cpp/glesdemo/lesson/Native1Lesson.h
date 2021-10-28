#ifndef OPEN_GL_LESSON_NATIVE_1LESSON_H
#define OPEN_GL_LESSON_NATIVE_1LESSON_H
#include "../../utils/GLUtils.h"

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

class Native1Lesson {
public:
	Native1Lesson();

	~Native1Lesson();

	void create();

	void change(int width, int height);

	void draw();

	void drawTriangle(GLfloat* vtxBuf, GLint vtxStride, GLint numIndices, GLushort* indices);

private:
	Matrix* mViewMatrix;
	Matrix* mModelMatrix;
	Matrix* mProjectionMatrix;
	Matrix* mMVPMatrix;

	GLuint mProgram;

	GLuint mMVPMatrixHandle;
};

#endif //OPEN_GL_LESSON_NATIVE_1LESSON_H
