#ifndef OPENGLLESSON_NATIVE3LESSON_H
#define OPENGLLESSON_NATIVE3LESSON_H

#include "../../utils/GLUtils.h"

class Native3Lesson {

public:

    Native3Lesson();

    ~Native3Lesson();

    void create();

    void change(int width, int height);

    void draw();

private:

    GLsizei mWidth;
    GLsizei mHeight;

    Matrix *mViewMatrix;
    Matrix *mModelMatrix;
    Matrix *mProjectionMatrix;
    Matrix *mMVPMatrix;
    Matrix *mLightModelMatrix;

    GLuint mMVPMatrixHandle;
    GLuint mMVMatrixHandle;
    GLuint mLightPosHandle;
    GLuint mPositionHandle;
    GLuint mColorHandle;
    GLuint mNormalHandle;

    GLuint mPerVertexProgramHandle;
    GLuint mPointProgramHandle;

    float mLightPosInModelSpace[4];
    float mLightPosInWorldSpace[4];
    float mLightPosInEyeSpace[4];

    void drawCube();

    void drawLight();
};

#endif //OPENGLLESSON_NATIVE3LESSON_H
