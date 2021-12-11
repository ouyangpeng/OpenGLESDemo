//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_TEXTUREWRAP_H
#define OPENGLESDEMO_TEXTUREWRAP_H

#include <GLBaseSample.h>

class TextureWrap : public GLBaseSample {
public:
    TextureWrap() = default;

    virtual ~TextureWrap() = default;

    virtual void create();

    virtual void draw();

    virtual void shutdown();

private:
    // Sampler location
    GLint samplerLoc;

    // Offset location
    GLint offsetLoc;

    // Texture handle
    GLuint textureId;

    static GLuint CreateTexture2D();

    static GLubyte *GenCheckImage(int width, int height, int checkSize);
};

#endif //OPENGLESDEMO_TEXTUREWRAP_H
