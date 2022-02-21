//
// Created by OuyangPeng on 2021/12/13.
//

#ifndef OPENGLESDEMO_TimeWatermarkStickerSample_H
#define OPENGLESDEMO_TimeWatermarkStickerSample_H

#include <vector>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>


class TimeWatermarkStickerSample : public GLBaseSample {

public:
    TimeWatermarkStickerSample();

    virtual ~TimeWatermarkStickerSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                         float scale, glm::vec3 transVec3,float ratio) const;

private:
    GLuint m_TextureId;
    GLuint m_StickerTextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLuint m_VaoId;
    GLuint m_VboIds[3];

    NativeImage m_RenderImage;
    NativeImage m_StickerRenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};

#endif //OPENGLESDEMO_TimeWatermarkStickerSample_H
