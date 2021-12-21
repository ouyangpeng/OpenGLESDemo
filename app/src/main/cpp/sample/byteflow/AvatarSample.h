//
// Created by OuyangPeng on 2021/12/21.
//

#ifndef OPENGLESDEMO_AVATARSAMPLE_H
#define OPENGLESDEMO_AVATARSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

#define RENDER_IMG_NUM 3

class AvatarSample : public GLBaseSample {
public:
    AvatarSample();

    virtual ~AvatarSample();

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float transX, float transY, float ratio) const;

    virtual void SetGravityXY(float x, float y);


private:
    GLuint m_TextureIds[RENDER_IMG_NUM];
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImages[RENDER_IMG_NUM];
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
    float m_TransX;
    float m_TransY;

    int m_FrameIndex;

    GLuint m_BlurProgramObj;
};


#endif //OPENGLESDEMO_AVATARSAMPLE_H
