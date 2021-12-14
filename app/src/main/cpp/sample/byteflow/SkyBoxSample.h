//
// Created by OuyangPeng on 2021/12/14.
//

#ifndef OPENGLESDEMO_SKYBOXSAMPLE_H
#define OPENGLESDEMO_SKYBOXSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class SkyBoxSample : public GLBaseSample{
public:
    SkyBoxSample();

    virtual ~SkyBoxSample();

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float scale, float ratio);

private:
    GLuint m_TextureId;

    GLuint m_CubeProgramObj;

    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLint m_CubeSamplerLoc;
    GLint m_CubeMVPMatLoc;
    GLint m_CubeModelMatLoc;
    GLint m_ViewPosLoc;

    GLuint m_CubeVaoId;
    GLuint m_CubeVboId;
    GLuint m_SkyBoxVaoId;
    GLuint m_SkyBoxVboId;

    NativeImage m_RenderImage;
    NativeImage m_pSkyBoxRenderImg[6];

    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;
};

static GLfloat skyboxCubeVertices[] = {
        //position           //normal
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
};

static GLfloat skyboxVertices[] = {
        // Positions
        -2.0f, 2.0f, -2.0f,
        -2.0f, -2.0f, -2.0f,
        2.0f, -2.0f, -2.0f,
        2.0f, -2.0f, -2.0f,
        2.0f, 2.0f, -2.0f,
        -2.0f, 2.0f, -2.0f,

        -2.0f, -2.0f, 2.0f,
        -2.0f, -2.0f, -2.0f,
        -2.0f, 2.0f, -2.0f,
        -2.0f, 2.0f, -2.0f,
        -2.0f, 2.0f, 2.0f,
        -2.0f, -2.0f, 2.0f,

        2.0f, -2.0f, -2.0f,
        2.0f, -2.0f, 2.0f,
        2.0f, 2.0f, 2.0f,
        2.0f, 2.0f, 2.0f,
        2.0f, 2.0f, -2.0f,
        2.0f, -2.0f, -2.0f,

        -2.0f, -2.0f, 2.0f,
        -2.0f, 2.0f, 2.0f,
        2.0f, 2.0f, 2.0f,
        2.0f, 2.0f, 2.0f,
        2.0f, -2.0f, 2.0f,
        -2.0f, -2.0f, 2.0f,

        -2.0f, 2.0f, -2.0f,
        2.0f, 2.0f, -2.0f,
        2.0f, 2.0f, 2.0f,
        2.0f, 2.0f, 2.0f,
        -2.0f, 2.0f, 2.0f,
        -2.0f, 2.0f, -2.0f,

        -2.0f, -2.0f, -2.0f,
        -2.0f, -2.0f, 2.0f,
        2.0f, -2.0f, -2.0f,
        2.0f, -2.0f, -2.0f,
        -2.0f, -2.0f, 2.0f,
        2.0f, -2.0f, 2.0f
};

#endif //OPENGLESDEMO_SKYBOXSAMPLE_H
