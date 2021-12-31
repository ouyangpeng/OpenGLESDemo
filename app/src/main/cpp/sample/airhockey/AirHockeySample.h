//
// Created by OuyangPeng on 2021/12/30.
//

#ifndef OPENGLESDEMO_AIRHOCKEYSAMPLE_H
#define OPENGLESDEMO_AIRHOCKEYSAMPLE_H

#include <GLBaseSample.h>

class AirHockeySample : public GLBaseSample {
public:
    AirHockeySample() = default;

    virtual ~AirHockeySample() = default;

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

private:
    GLint uColorLocation;
};


#endif //OPENGLESDEMO_AIRHOCKEYSAMPLE_H
