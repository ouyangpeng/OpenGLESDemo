//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_MULTITEXTURE_H
#define OPENGLESDEMO_MULTITEXTURE_H

#include "../../utils/GLUtils.h"

class MultiTexture {
public:
    MultiTexture();

    ~MultiTexture();

    void create();

    void change(int width, int height);

    void draw();

    void shutdown();

private:
    // Handle to a program object
    GLuint programObject;

    // Sampler locations
    GLint baseMapLoc;
    GLint lightMapLoc;

    // Texture handle
    GLuint baseMapTexId;
    GLuint lightMapTexId;

    int mWidth;
    int mHeight;
};


#endif //OPENGLESDEMO_MULTITEXTURE_H
