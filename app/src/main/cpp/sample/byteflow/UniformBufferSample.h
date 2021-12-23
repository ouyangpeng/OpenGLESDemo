//
// Created by OuyangPeng on 2021/12/23.
//

#ifndef OPENGLESDEMO_UNIFORMBUFFERSAMPLE_H
#define OPENGLESDEMO_UNIFORMBUFFERSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class UniformBufferSample  : public GLBaseSample {
public:
    UniformBufferSample();

    virtual ~UniformBufferSample();

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_UboId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLESDEMO_UNIFORMBUFFERSAMPLE_H
