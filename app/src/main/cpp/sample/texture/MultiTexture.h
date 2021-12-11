//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_MULTITEXTURE_H
#define OPENGLESDEMO_MULTITEXTURE_H

#include <GLBaseSample.h>

#define MULTI_TEXTURE_VERTEX_POS_INDX               0
#define MULTI_TEXTURE_VERTEX_TEXCOORD_INDX          1

class MultiTexture : public GLBaseSample {
public:
    MultiTexture() = default;

    virtual ~MultiTexture() = default;

    virtual void create();

    virtual void draw();

    virtual void shutdown();

private:
    // Sampler locations
    GLint baseMapLoc;
    GLint lightMapLoc;

    // Texture handle
    GLuint baseMapTexId;
    GLuint lightMapTexId;
};


#endif //OPENGLESDEMO_MULTITEXTURE_H
