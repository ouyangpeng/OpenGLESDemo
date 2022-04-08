//
// Created by OuyangPeng on 2021/12/10.
//

#ifndef OPENGLESDEMO_ROTATETEXTURE_H
#define OPENGLESDEMO_ROTATETEXTURE_H

#include <GLBaseSample.h>
#include <glm/detail/type_mat4x4.hpp>

#define CS_VERTEX_POS_INDX  0
#define CS_TEXTURE_POS_INDX 1

class RotateTexture : public GLBaseSample {
public:
    RotateTexture();

    virtual ~RotateTexture();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    NativeImage m_RenderImage;

    long mLastTime;
    // 旋转 angle
    GLfloat angle;

    void update(float deltaTime);

    float getDeltaTime();

    float translateToRadianAngle(int rotateAngle);

    void updateVertexCoordinates();
};
#endif //OPENGLESDEMO_RotateTexture_H
