//
// Created by OuyangPeng on 2021/10/24.
//

#ifndef OPENGLESDEMO_SIMPLETEXTURE2D_H
#define OPENGLESDEMO_SIMPLETEXTURE2D_H

#include <GLBaseSample.h>

#define SIMPLE_TEXTURE_2D_VERTEX_POS_INDEX              0
#define SIMPLE_TEXTURE_2D_VERTEX_TEXTCOORD_INDEX        1

class SimpleTexture2D : public GLBaseSample {

public:
    SimpleTexture2D() = default;

    virtual ~SimpleTexture2D() = default;

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

private:
    // Sampler location
    GLint samplerLoc;

    // Texture handle
    GLuint textureId;

    static GLuint CreateSimpleTexture2D();
};

#endif //OPENGLESDEMO_SIMPLETEXTURE2D_H
