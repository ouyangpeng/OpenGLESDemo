//
// Created by OuyangPeng on 2021/10/25.
//
#pragma once

#include "../../utils/GLUtils.h"

#define POSITION_LOC  0

class TerrainRender : public BaseGLSample {
public:
    TerrainRender();

    virtual ~TerrainRender();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();

private:

// Uniform locations
    GLint mvpLoc;
    GLint lightDirectionLoc;

    // Sampler location
    GLint samplerLoc;

    // Texture handle
    GLuint textureId;

    // VBOs
    GLuint positionVBO;
    GLuint indicesIBO;

    // Number of indices
    int numIndices;

    // dimension of grid
    int gridSize;

    // MVP matrix
    ESMatrix mvpMatrix;

    void initMVP();
};
