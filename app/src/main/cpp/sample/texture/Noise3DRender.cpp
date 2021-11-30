//
// Created by OuyangPeng on 2021/10/25.
//

#include <cstdlib>
#include "Noise3DRender.h"


void Noise3DRender::create() {
    GLUtils::printGLInfo();

    // Create the 3D texture
    Create3DNoiseTexture(64, 5.0f);

    // Main Program
    const char *vertex = GLUtils::openTextFile(
            "vertex/vertex_shader_noise3d.glsl");
    const char *fragment = GLUtils::openTextFile(
            "fragment/fragment_shader_noise3d.glsl");

    // Set program handles
    mProgram = GLUtils::createProgram(&vertex, &fragment);

    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }

    // Get the uniform locations
    mvpLoc = glGetUniformLocation(mProgram, "u_mvpMatrix");
    mvLoc = glGetUniformLocation(mProgram, "u_mvMatrix");
    noiseTexLoc = glGetUniformLocation(mProgram, "s_noiseTex");
    fogMinDistLoc = glGetUniformLocation(mProgram, "u_fogMinDist");
    fogMaxDistLoc = glGetUniformLocation(mProgram, "u_fogMaxDist");
    fogColorLoc = glGetUniformLocation(mProgram, "u_fogColor");
    timeLoc = glGetUniformLocation(mProgram, "u_time");

    // Generate the vertex data
    numIndices = esGenCube(3.0, &vertices,
                           nullptr, &texCoords, &indices);

    // Starting rotation angle for the cube
    angle = 0.0f;
    curTime = 0.0f;

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void Noise3DRender::draw() {
    // 每次更新一下
    update(getDeltaTime());

    // Set the viewport
    glViewport(0, 0, mWidth, mHeight);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram(mProgram);

    // Load the vertex position
    glVertexAttribPointer(ATTRIB_LOCATION_POS, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), vertices);

    glEnableVertexAttribArray(ATTRIB_LOCATION_POS);

    // Set the vertex color to red
    glVertexAttrib4f(ATTRIB_LOCATION_COLOR, 1.0f, 0.0f, 0.0f, 1.0f);

    // Bind the texture coordinates
    glVertexAttribPointer(ATTRIB_LOCATION_TEXCOORD, 2, GL_FLOAT,
                          GL_FALSE, 2 * sizeof(GLfloat), texCoords);

    glEnableVertexAttribArray(ATTRIB_LOCATION_TEXCOORD);

    // Load the matrices
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (GLfloat *) &mvpMatrix.m[0][0]);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (GLfloat *) &mvMatrix.m[0][0]);

    // Load other uniforms
    {
        float fogColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        float fogMinDist = 2.75f;
        float fogMaxDist = 4.0f;
        glUniform1f(fogMinDistLoc, fogMinDist);
        glUniform1f(fogMaxDistLoc, fogMaxDist);

        glUniform4fv(fogColorLoc, 1, fogColor);
        glUniform1f(timeLoc, curTime * 0.1f);
    }

    // Bind the 3D texture
    glUniform1i(noiseTexLoc, 0);
    glBindTexture(GL_TEXTURE_3D, textureId);

    // Draw the cube
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);

}

void Noise3DRender::shutdown() {
    if (vertices != nullptr) {
        free(vertices);
    }

    if (indices != nullptr) {
        free(indices);
    }

    if (texCoords != nullptr) {
        free(texCoords);
    }

    // Delete texture object
    glDeleteTextures(1, &textureId);

    // Delete program object
    glDeleteProgram(mProgram);
}

void Noise3DRender::update(float deltaTime) {

    ESMatrix perspective;
    float aspect;

    curTime += deltaTime;

    // Compute the window aspect ratio
    aspect = (GLfloat) mWidth / (GLfloat) mHeight;

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity(&perspective);
    esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);

    // Generate a model view matrix to rotate/translate the cube
    esMatrixLoadIdentity(&mvMatrix);

    // Translate away from the viewer
    esTranslate(&mvMatrix, 0.0, -2.5, -2.5);

    // Rotate the cube
    esRotate(&mvMatrix, angle, 1.0, 0.0, 1.0);

    // Compute the final MVP by multiplying the
    // modevleiw and perspective matrices together
    esMatrixMultiply(&mvpMatrix, &mvMatrix, &perspective);
}

float Noise3DRender::getDeltaTime() {
    if (mLastTime == 0) {
        mLastTime = TimeUtils::currentTimeMillis();
    }
    long currentTime = TimeUtils::currentTimeMillis();
    long elapsedTime = currentTime - mLastTime;
    float deltaTime = (float) elapsedTime / 1000.0f;
    mLastTime = currentTime;
    return deltaTime;
}