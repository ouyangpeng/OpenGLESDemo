//
// Created by OuyangPeng on 2021/12/27.
//

#ifndef OPENGLESDEMO_GLTRANSITIONEXAMPLE_H
#define OPENGLESDEMO_GLTRANSITIONEXAMPLE_H

#include <GLBaseSample.h>
#include <glm/gtc/matrix_transform.hpp>

#define BF_IMG_NUM    6
#define BF_LOOP_COUNT 200

class GLTransitionExample : public GLBaseSample {
public:
    GLTransitionExample(int renderSampleType);

    virtual ~GLTransitionExample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:
    int mRenderSampleType;
    const char *mFragmentShaderPath;

    GLuint m_TextureIds[BF_IMG_NUM];
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImages[BF_IMG_NUM];
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

    int m_frameIndex;
    int m_loopCount;

};


#endif //OPENGLESDEMO_GLTRANSITIONEXAMPLE_H
