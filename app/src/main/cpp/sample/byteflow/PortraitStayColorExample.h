//
// Created by OuyangPeng on 2021/12/25 0025.
//

#ifndef OPENGLESDEMO_PORTRAITSTAYCOLOREXAMPLE_H
#define OPENGLESDEMO_PORTRAITSTAYCOLOREXAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class PortraitStayColorExample : public GLBaseSample {

public:
    PortraitStayColorExample();

    virtual ~PortraitStayColorExample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:
    GLuint m_TextureId;
    GLuint m_GrayTexId;
    GLuint m_MappingTexId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImage;
    NativeImage m_GrayImage;
    NativeImage m_MappingImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

    int m_frameIndex;
};


#endif //OPENGLESDEMO_PORTRAITSTAYCOLOREXAMPLE_H
