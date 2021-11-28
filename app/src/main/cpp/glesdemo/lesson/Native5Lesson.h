#ifndef OPENGLLESSON_NATIVE5LESSON_H
#define OPENGLLESSON_NATIVE5LESSON_H

#include "../../utils/GLUtils.h"

class Native5Lesson : public BaseGLSample{

public:

    Native5Lesson();

    virtual ~Native5Lesson();
    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();

    virtual void switchBlendingMode();

private:

    //
    float *mCubePositionData;
    float *mCubeColorData;

    //
    GLsizei mWidth;
    GLsizei mHeight;

    // Matrix
    Matrix *mModelMatrix;
    Matrix *mViewMatrix;
    Matrix *mProjectionMatrix;
    Matrix *mMVPMatrix;

    // Handle
    GLuint mMVPMatrixHandle;
    GLuint mPositionHandle;
    GLuint mColorHandle;

    GLuint mProgramHandle;

    bool mBending;

    void drawCube();

    void createCubeData();
};


#endif //OPENGLLESSON_NATIVE5LESSON_H
