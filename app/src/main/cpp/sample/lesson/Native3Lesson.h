#ifndef OPENGLLESSON_NATIVE3LESSON_H
#define OPENGLLESSON_NATIVE3LESSON_H

#include <GLBaseSample.h>
#include "lesson_cube_data.h"

class Native3Lesson : public GLBaseSample {

public:
    Native3Lesson();

    virtual ~Native3Lesson();

    virtual void Create();

    virtual void Change(int width, int height);

    virtual void Draw();

    virtual void Shutdown();

private:
    Matrix *mViewMatrix;
    Matrix *mModelMatrix;
    Matrix *mProjectionMatrix;
    Matrix *mMVPMatrix;
    Matrix *mLightModelMatrix;

    GLuint mMVPMatrixHandle;
    GLuint mMVMatrixHandle;
    GLuint mLightPosHandle;

    GLuint mPerVertexProgramHandle;
    GLuint mPointProgramHandle;

    float mLightPosInModelSpace[4];
    float mLightPosInWorldSpace[4];
    float mLightPosInEyeSpace[4];

    void drawCube();

    void drawLight();
};

#endif //OPENGLLESSON_NATIVE3LESSON_H
