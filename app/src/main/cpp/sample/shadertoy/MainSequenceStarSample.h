//
// Created by OuyangPeng on 2021/12/15.
//

#ifndef OPENGLESDEMO_MAINSEQUENCESTARSAMPLE_H
#define OPENGLESDEMO_MAINSEQUENCESTARSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

#define MAIN_SEQUENCE_STAR_RENDER_IMG_NUM 2

class MainSequenceStarSample : public GLBaseSample {

public:
    MainSequenceStarSample();

    virtual ~MainSequenceStarSample();

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:
    // Texture handle
    GLuint m_TextureIds[MAIN_SEQUENCE_STAR_RENDER_IMG_NUM];
    NativeImage m_RenderImages[MAIN_SEQUENCE_STAR_RENDER_IMG_NUM];

    GLint m_SamplerLocIChannels[MAIN_SEQUENCE_STAR_RENDER_IMG_NUM];
    GLint m_TimeLoc;
    GLint m_SizeLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

};


#endif //OPENGLESDEMO_MAINSEQUENCESTARSAMPLE_H
