//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_TEXTUREWRAP_H
#define OPENGLESDEMO_TEXTUREWRAP_H

#include "../../utils/GLUtils.h"
class TextureWrap {
public:
    TextureWrap();

    ~TextureWrap();

    void create();

    void change(int width, int height);

    void draw();

    void shutdown();

private:
    // Handle to a program object
    GLuint programObject;

    // Sampler location
    GLint samplerLoc;

    // Offset location
    GLint offsetLoc;

    // Texture handle
    GLuint textureId;

    int mWidth;
    int mHeight;

    GLuint CreateTexture2D();

    GLubyte *GenCheckImage(int width, int height, int checkSize);
};
#endif //OPENGLESDEMO_TEXTUREWRAP_H
