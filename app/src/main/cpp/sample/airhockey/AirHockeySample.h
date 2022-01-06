//
// Created by OuyangPeng on 2021/12/30.
//

#ifndef OPENGLESDEMO_AIRHOCKEYSAMPLE_H
#define OPENGLESDEMO_AIRHOCKEYSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

using namespace glm;

class AirHockeySample : public GLBaseSample {
public:
    AirHockeySample() = default;

    virtual ~AirHockeySample() = default;

    virtual void Create();

    virtual void Change(int width, int height);

    virtual void LoadImage(NativeImage *pImage);

    virtual void Draw();

    virtual void Shutdown();

private:
    GLuint m_TextureId;

    GLint m_SamplerLoc;

    GLint m_TableMVPMatLoc;
    GLint m_MalletMVPMatLoc;

    mat4 m_MVPMatrix;

    float aspectRatio;

    NativeImage m_RenderImage;

    GLuint m_MalletProgramObj;
};


#endif //OPENGLESDEMO_AIRHOCKEYSAMPLE_H
