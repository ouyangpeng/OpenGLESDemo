//
// Created by OuyangPeng on 2021/12/13.
//

#ifndef OPENGLESDEMO_STENCILTESTINGSAMPLE_H
#define OPENGLESDEMO_STENCILTESTINGSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class StencilTestingSample : public GLBaseSample
{
public:
    StencilTestingSample();

    virtual ~StencilTestingSample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    static void UpdateMatrix(glm::mat4 &mvpMatrix, glm::mat4 &modelMatrix, int angleXRotate, int angleYRotate, float scale, glm::vec3 transVec3, float ratio);

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
    GLuint m_VboIds[1];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;

    int m_AngleX;
    int m_AngleY;

    GLuint m_FrameProgramObj;

    void drawContainer(float ratio);

    void drawScaledUpContainer(float ratio);
};
#endif //OPENGLESDEMO_STENCILTESTINGSAMPLE_H
