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

    virtual void Draw();

    virtual void Shutdown();

private:
    GLint m_MVPMatLoc;
    mat4 m_MVPMatrix;
    float aspectRatio;
};


#endif //OPENGLESDEMO_AIRHOCKEYSAMPLE_H
