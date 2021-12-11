//
// Created by OuyangPeng on 2021/10/24.
//
#pragma once

#include <GLBaseSample.h>

#define SIMPLE_TEXTURE_CUBE_MAP_VERTEX_POS_INDEX            0
#define SIMPLE_TEXTURE_CUBE_MAP_VERTEX_NORMAL_INDEX         1

class SimpleTextureCubeMap : public GLBaseSample {

public:
    SimpleTextureCubeMap() = default;

    virtual ~SimpleTextureCubeMap() = default;

    virtual void create();

    virtual void draw();

    virtual void shutdown();

private:
    // Sampler location
    GLint samplerLoc;

    // Texture handle
    GLuint textureId;

    // Vertex data
    int numIndices;
    GLfloat *vertices;
    GLfloat *normals;
    GLuint *indices;

    static GLuint CreateSimpleTextureCubemap();
};
