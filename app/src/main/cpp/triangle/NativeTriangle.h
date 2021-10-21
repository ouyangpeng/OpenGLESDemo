#pragma once
#include <GLES2/gl2.h>
#include "../graphics/Matrix.h"

class NativeTriangle {

public:
    NativeTriangle();

    ~NativeTriangle();

    void create();

    void draw();

private:
    GLuint mProgram;
};
