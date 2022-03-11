//
// Created by OuyangPeng on 2022/3/10.
//

#ifndef OPENGLESDEMO_GREENSCREENMATTINGMIX_H
#define OPENGLESDEMO_GREENSCREENMATTINGMIX_H

#include <GLBaseSample.h>

#define GREEN_SCREEN_RENDER_IMG_NUM 2

class GreenScreenMattingMix : public GLBaseSample {

public:
    GreenScreenMattingMix();

    virtual ~GreenScreenMattingMix();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:

    GLuint m_TextureIds[GREEN_SCREEN_RENDER_IMG_NUM];
    NativeImage m_RenderImages[GREEN_SCREEN_RENDER_IMG_NUM];

    GLint m_SamplerLoc;
    GLint s_TextureMapBackgroundSamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];

    glm::mat4 m_MVPMatrix;
    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};
#endif //OPENGLESDEMO_GREENSCREENMATTINGMIX_H
