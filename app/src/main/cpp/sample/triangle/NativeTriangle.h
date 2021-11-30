#pragma once

#include <BaseGLSample.h>

class NativeTriangle : public BaseGLSample {

#define VERTEX_POS_INDX       0

public:
    NativeTriangle() = default;

    virtual ~NativeTriangle() = default;

    virtual void create();

    virtual void draw();

    virtual void shutdown();
};
