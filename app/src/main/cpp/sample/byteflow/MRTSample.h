//
// Created by OuyangPeng on 2021/12/21 0021.
//

#ifndef OPENGLESDEMO_MRTSAMPLE_H
#define OPENGLESDEMO_MRTSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

#define ATTACHMENT_NUM 4

class MRTSample : public GLBaseSample {
public:
    MRTSample();

    virtual ~MRTSample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

    bool InitFBO();

private:
    GLuint m_TextureId;
    GLuint m_MRTProgramObj;
    GLuint m_AttachTexIds[ATTACHMENT_NUM];
    GLuint m_FBO;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLESDEMO_MRTSAMPLE_H
