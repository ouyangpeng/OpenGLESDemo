#ifndef OPENGLLESSON_NATIVEFOURLESSON_H
#define OPENGLLESSON_NATIVEFOURLESSON_H

#include <GLBaseSample.h>
#include "lesson_cube_data.h"

class Native4Lesson : public GLBaseSample {

public:
    Native4Lesson();

    virtual ~Native4Lesson();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();

private:
    Matrix *mViewMatrix;
    Matrix *mModelMatrix;
    Matrix *mProjectionMatrix;
    Matrix *mMVPMatrix;
    Matrix *mLightModelMatrix;

    GLuint mMVPMatrixHandle;
    GLuint mMVMatrixHandle;
    GLuint mLightPosHandle;
    GLuint mTextureUniformHandle;

    GLuint mTextureDataHandle;

    GLuint mPerVertexProgramHandle;
    GLuint mPointProgramHandle;

    float mLightPosInModelSpace[4];
    float mLightPosInWorldSpace[4];
    float mLightPosInEyeSpace[4];

    void drawCube();

    void drawLight();
};

#endif //OPENGLLESSON_NATIVEFOURLESSON_H
