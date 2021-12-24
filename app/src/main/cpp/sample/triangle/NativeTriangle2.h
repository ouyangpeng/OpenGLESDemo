#pragma once

#include <GLBaseSample.h>

class NativeTriangle2 : public GLBaseSample {

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

public:
    NativeTriangle2() = default;

    virtual ~NativeTriangle2() = default;

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();
};
