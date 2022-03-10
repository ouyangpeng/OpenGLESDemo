//
// Created by OuyangPeng on 2022/3/10.
//

#ifndef OPENGLESDEMO_GREENSCREENMATTING_H
#define OPENGLESDEMO_GREENSCREENMATTING_H

#include <GLBaseSample.h>
class GreenScreenMatting : public GLBaseSample {

public:
    GreenScreenMatting();

    virtual ~GreenScreenMatting();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:
    GLuint m_TextureId;
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
#endif //OPENGLESDEMO_GREENSCREENMATTING_H
