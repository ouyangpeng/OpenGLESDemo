#ifndef OPEN_GL_LESSON_NATIVE_1LESSON_H
#define OPEN_GL_LESSON_NATIVE_1LESSON_H

#include <GLES2/gl2.h>
#include "../graphics/Matrix.h"

class Native1Lesson {

public:
    Native1Lesson();

    ~Native1Lesson();

    void create();

    void change(int width, int height);

    void draw();

    void drawTriangle(GLfloat *verticesData);

private:
    Matrix *mViewMatrix;
    Matrix *mModelMatrix;
    Matrix *mProjectionMatrix;
    Matrix *mMVPMatrix;

    GLuint mProgram;

    GLuint mMVPMatrixHandle;
    GLuint mPositionHandle;
    GLuint mColorHandle;
};

#endif //OPEN_GL_LESSON_NATIVE_1LESSON_H
