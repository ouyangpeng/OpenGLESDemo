//
// Created by OuyangPeng on 2021/12/11 0011.
//

#ifndef OPENGLESDEMO_INSTANCING3DSAMPLE_H
#define OPENGLESDEMO_INSTANCING3DSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class Instancing3DSample : public GLBaseSample {
public:
    Instancing3DSample();

    virtual ~Instancing3DSample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    static void UpdateMatrix(glm::mat4 &mvpMatrix, glm::mat4 &modelMatrix,
                      int angleXRotate, int angleYRotate,float scale,
                      glm::vec3 transVec3, float ratio);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLint m_ModelMatrixLoc;
    GLint m_ViewPosLoc;

    GLint m_LightPosition;
    GLint m_LightColor;
    GLint m_LightDirection;
    GLint m_LightCutOff;
    GLint m_LightOuterCutOff;
    GLint m_LightConstant;
    GLint m_LightLinear;
    GLint m_LightQuadratic;

    GLuint m_VaoId;
    GLuint m_VboIds[2];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;

};

static GLfloat i3d_vertices[] = {
         //position            //texture coord  //normal
        -0.09f, -0.09f, -0.09f,   0.0f, 0.0f,      0.0f,  0.0f, -1.0f,
        0.09f, -0.09f, -0.09f,   1.0f, 0.0f,      0.0f,  0.0f, -1.0f,
        0.09f,  0.09f, -0.09f,   1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        0.09f,  0.09f, -0.09f,   1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        -0.09f,  0.09f, -0.09f,   0.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        -0.09f, -0.09f, -0.09f,   0.0f, 0.0f,      0.0f,  0.0f, -1.0f,

        -0.09f, -0.09f, 0.09f,    0.0f, 0.0f,      0.0f,  0.0f,  1.0f,
        0.09f, -0.09f, 0.09f,    1.0f, 0.0f,      0.0f,  0.0f,  1.0f,
        0.09f,  0.09f, 0.09f,    1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        0.09f,  0.09f, 0.09f,    1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        -0.09f,  0.09f, 0.09f,    0.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        -0.09f, -0.09f, 0.09f,    0.0f, 0.0f,      0.0f,  0.0f,  1.0f,

        -0.09f,  0.09f,  0.09f,   1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
        -0.09f,  0.09f, -0.09f,   1.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.09f, -0.09f, -0.09f,   0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.09f, -0.09f, -0.09f,   0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.09f, -0.09f,  0.09f,   0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
        -0.09f,  0.09f,  0.09f,   1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,

        0.09f,  0.09f,  0.09f,   1.0f, 0.0f,      1.0f,  0.0f,  0.0f,
        0.09f,  0.09f, -0.09f,   1.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.09f, -0.09f, -0.09f,   0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.09f, -0.09f, -0.09f,   0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.09f, -0.09f,  0.09f,   0.0f, 0.0f,      1.0f,  0.0f,  0.0f,
        0.09f,  0.09f,  0.09f,   1.0f, 0.0f,      1.0f,  0.0f,  0.0f,

        -0.09f, -0.09f, -0.09f,   0.0f, 1.0f,      0.0f, -1.0f,  0.0f,
        0.09f, -0.09f, -0.09f,   1.0f, 1.0f,      0.0f, -1.0f,  0.0f,
        0.09f, -0.09f,  0.09f,   1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        0.09f, -0.09f,  0.09f,   1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        -0.09f, -0.09f,  0.09f,   0.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        -0.09f, -0.09f, -0.09f,   0.0f, 1.0f,      0.0f, -1.0f,  0.0f,

        -0.09f, 0.09f, -0.09f,    0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
        0.09f, 0.09f, -0.09f,    1.0f, 1.0f,      0.0f,  1.0f,  0.0f,
        0.09f, 0.09f,  0.09f,    1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        0.09f, 0.09f,  0.09f,    1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        -0.09f, 0.09f,  0.09f,    0.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        -0.09f, 0.09f, -0.09f,    0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
};
#endif //OPENGLESDEMO_INSTANCING3DSAMPLE_H
