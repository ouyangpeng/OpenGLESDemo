#pragma once
#include "../../utils/GLUtils.h"
#include "../BaseGLSample.h"

class NativeTriangle : public BaseGLSample {

public:
    NativeTriangle();

    virtual ~NativeTriangle();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();
};
