//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_TEXTUREWRAP_H
#define OPENGLESDEMO_TEXTUREWRAP_H

#include <BaseGLSample.h>
class TextureWrap : public BaseGLSample{
public:
    TextureWrap();

    virtual ~TextureWrap();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();

private:
    // Sampler location
    GLint samplerLoc;

    // Offset location
    GLint offsetLoc;

    // Texture handle
    GLuint textureId;

    GLuint CreateTexture2D();

    GLubyte *GenCheckImage(int width, int height, int checkSize);
};
#endif //OPENGLESDEMO_TEXTUREWRAP_H
