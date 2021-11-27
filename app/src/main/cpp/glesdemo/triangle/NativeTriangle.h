#pragma once

#include "../../utils/GLUtils.h"

class NativeTriangle : public BaseGLSample {

#define VERTEX_POS_INDX       0

public:
    NativeTriangle();

    virtual ~NativeTriangle();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();
};
