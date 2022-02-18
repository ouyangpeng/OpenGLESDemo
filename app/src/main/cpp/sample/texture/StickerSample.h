//
// Created by OuyangPeng on 2021/12/13.
//

#ifndef OPENGLESDEMO_STICKERSAMPLE_H
#define OPENGLESDEMO_STICKERSAMPLE_H

#include <vector>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

#define RENDER_IMG_NUM 4

class StickerSample : public GLBaseSample {

public:
    StickerSample();

    virtual ~StickerSample();

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
    GLuint m_StickerTextureIds[RENDER_IMG_NUM];
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLuint m_VaoId;
    GLuint m_VboIds[3];

    NativeImage m_RenderImage;
    NativeImage m_StickerRenderImages[RENDER_IMG_NUM];
    glm::mat4 m_MVPMatrix;

    std::vector<glm::vec3> windowsTrans;
    std::map<GLfloat, glm::vec3> sorted;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};

#endif //OPENGLESDEMO_STICKERSAMPLE_H
