//
// Created by OuyangPeng on 2021/11/26.
//

#ifndef OPENGLESDEMO_BASEGLSAMPLE_H
#define OPENGLESDEMO_BASEGLSAMPLE_H

#include "../utils/GLUtils.h"
#include "BaseGLSampleType.h"

class BaseGLSample {
public:
    BaseGLSample() {
        mProgram = 0;
        mWidth = 0;
        mHeight = 0;
    }

    virtual ~BaseGLSample() {

    }

    virtual void create() = 0;

    virtual void change(int width, int height) = 0;

    virtual void draw() = 0;

    virtual void shutdown() = 0;

protected:
    GLuint mProgram;
    int mWidth;
    int mHeight;

    const char *VERTEX_SHADER;
    const char *FRAGMENT_SHADER;
};


#endif //OPENGLESDEMO_BASEGLSAMPLE_H
