//
// Created by OuyangPeng on 2021/12/15.
//

#ifndef OPENGLESDEMO_BEATINGHEARTSAMPLE_H
#define OPENGLESDEMO_BEATINGHEARTSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class BeatingHeartSample : public GLBaseSample {

public:
    BeatingHeartSample();

    virtual ~BeatingHeartSample();

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_TimeLoc;
    GLint m_SizeLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[2];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLESDEMO_BEATINGHEARTSAMPLE_H
