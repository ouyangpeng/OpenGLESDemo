//
// Created by OuyangPeng on 2021/12/14.
//

#ifndef OPENGLESDEMO_PBOSAMPLE_H
#define OPENGLESDEMO_PBOSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class PBOSample : public GLBaseSample{
public:
    PBOSample();

    virtual ~PBOSample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

    bool CreateFrameBufferObj();

    void UploadPixels();

    void DownloadPixels();

private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_FboId;
    GLuint m_VaoIds[2] = {GL_NONE};
    GLuint m_VboIds[4] = {GL_NONE};;
    GLint m_SamplerLoc;
    NativeImage m_RenderImage;
    GLuint m_FboProgramObj;
    GLint m_FboSamplerLoc;
    GLint m_MVPMatrixLoc;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
    glm::mat4 m_MVPMatrix;

    GLuint m_UploadPboIds[2] = {GL_NONE};;
    GLuint m_DownloadPboIds[2] = {GL_NONE};;
    int m_FrameIndex;
};


#endif //OPENGLESDEMO_PBOSAMPLE_H
