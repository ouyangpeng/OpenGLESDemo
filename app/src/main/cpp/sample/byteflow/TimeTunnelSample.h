//
// Created by OuyangPeng on 2021/12/15.
//

#ifndef OPENGLESDEMO_TIMETUNNELSAMPLE_H
#define OPENGLESDEMO_TIMETUNNELSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class TimeTunnelSample : public GLBaseSample {
public:
    TimeTunnelSample();

    virtual ~TimeTunnelSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

    bool CreateFrameBufferObj();

private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_FboId;
    GLuint m_VaoIds[2] = {GL_NONE};
    GLuint m_VboIds[4] = {GL_NONE};
    GLint m_SamplerLoc;
    GLint m_ImgSizeLoc;
    GLint m_TimeLoc;
    NativeImage m_RenderImage;
    GLuint m_FboProgramObj;
    GLint m_FboSamplerLoc;
    GLint m_MVPMatrixLoc;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
    glm::mat4 m_MVPMatrix;

    int m_FrameIndex;
};


#endif //OPENGLESDEMO_TIMETUNNELSAMPLE_H
