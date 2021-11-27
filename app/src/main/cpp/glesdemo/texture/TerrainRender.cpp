//
// Created by OuyangPeng on 2021/10/25.
//

#include <cstdlib>
#include "TerrainRender.h"

TerrainRender::TerrainRender() {

}

TerrainRender::~TerrainRender() {

}

void TerrainRender::create() {
    GLUtils::printGLInfo();

    GLfloat *positions;
    GLuint *indices;

    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_terrainrender.glsl");
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_terrinrender.glsl");

    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }

    // Get the uniform locations
    mvpLoc = glGetUniformLocation(mProgram, "u_mvpMatrix");
    lightDirectionLoc = glGetUniformLocation(mProgram, "u_lightDirection");

    // Get the sampler location
    samplerLoc = glGetUniformLocation(mProgram, "s_texture");

    textureId = GLUtils::loadTgaTexture("texture/heightmap.tga");
    if (textureId == 0) {
        return;
    }

    // Generate the position and indices of a square grid for the base terrain
    gridSize = 200;
    numIndices = esGenSquareGrid(gridSize, &positions, &indices);

    // Index buffer for base terrain
    glGenBuffers(1, &indicesIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 numIndices * sizeof(GLuint),
                 indices,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    free(indices);

    // Position VBO for base terrain
    glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 gridSize * gridSize * sizeof(GLfloat) * 3,
                 positions,
                 GL_STATIC_DRAW);
    free(positions);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void TerrainRender::change(int width, int height) {
    mWidth = width;
    mHeight = height;
    LOGD("change() width = %d , height = %d\n", width, height)

    // Set the viewport
    glViewport(0, 0, mWidth, mHeight);
}

//
// Initialize the MVP matrix
//
void TerrainRender::initMVP() {
    ESMatrix perspective;
    ESMatrix modelview;
    float aspect;

    // Compute the window aspect ratio
    aspect = (GLfloat) mWidth / (GLfloat) mHeight;

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity(&perspective);
    esPerspective(&perspective, 60.0f, aspect, 0.1f, 20.0f);

    // Generate a model view matrix to rotate/translate the terrain
    esMatrixLoadIdentity(&modelview);

    // Center the terrain
    esTranslate(&modelview, -0.5f, -0.5f, -0.7f);

    // Rotate
    esRotate(&modelview, 45.0f, 1.0, 0.0, 0.0);

    // Compute the final MVP by multiplying the
    // modelview and perspective matrices together
    esMatrixMultiply(&mvpMatrix, &modelview, &perspective);
}

void TerrainRender::draw() {
    initMVP();

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(mProgram);

    // Load the vertex position
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(POSITION_LOC);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);

    // Bind the height map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Load the MVP matrix
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvpMatrix.m[0][0]);

    // Load the light direction
    glUniform3f(lightDirectionLoc, 0.86f, 0.14f, 0.49f);

    // Set the height map sampler to texture unit to 0
    glUniform1i(samplerLoc, 0);

    // Draw the grid
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
}

void TerrainRender::shutdown() {
    // Delete texture object
    glDeleteBuffers(1, &positionVBO);
    glDeleteBuffers(1, &indicesIBO);

    glDeleteProgram(mProgram);
}