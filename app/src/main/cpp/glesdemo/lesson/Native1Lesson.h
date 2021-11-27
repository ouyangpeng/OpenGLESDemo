#ifndef OPEN_GL_LESSON_NATIVE_1LESSON_H
#define OPEN_GL_LESSON_NATIVE_1LESSON_H
#include "../../utils/GLUtils.h"

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

class Native1Lesson : public BaseGLSample {
public:
	Native1Lesson();

	virtual ~Native1Lesson();

	virtual void create();

	virtual void change(int width, int height);

	virtual void draw();

	virtual void shutdown();

	void drawTriangle(GLfloat* vtxBuf, GLint vtxStride, GLint numIndices, GLushort* indices);

private:
	Matrix* mViewMatrix;
	Matrix* mModelMatrix;
	Matrix* mProjectionMatrix;
	Matrix* mMVPMatrix;

	GLuint mMVPMatrixHandle;
};

#endif //OPEN_GL_LESSON_NATIVE_1LESSON_H
