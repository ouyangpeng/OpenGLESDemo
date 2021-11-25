//
// Created by OuyangPeng on 2021/10/25.
//
#pragma once

#include "../../utils/GLUtils.h"

#define POSITION_LOC  0

class TerrainRender {
public:
    TerrainRender();

    ~TerrainRender();

    void create();

    void change(int width, int height);

    void draw();

    void shutdown();

private:
    // Handle to a program object
    GLuint programObject;
// Uniform locations
    GLint  mvpLoc;
    GLint  lightDirectionLoc;

    // Sampler location
    GLint samplerLoc;

    // Texture handle
    GLuint textureId;

    // VBOs
    GLuint positionVBO;
    GLuint indicesIBO;

    // Number of indices
    int    numIndices;

    // dimension of grid
    int    gridSize;

    // MVP matrix
    ESMatrix  mvpMatrix;

    int mWidth;
    int mHeight;

    void initMVP();
};
