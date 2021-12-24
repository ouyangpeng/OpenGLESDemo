//
// Created by OuyangPeng on 2021/12/19 0019.
//

#ifndef OPENGLESDEMO_FACESLENDERSAMPLE_H
#define OPENGLESDEMO_FACESLENDERSAMPLE_H


#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class FaceSlenderSample : public GLBaseSample {
public:
    FaceSlenderSample();

    virtual ~FaceSlenderSample();

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

    int m_FrameIndex;
};


#endif //OPENGLESDEMO_FACESLENDERSAMPLE_H
