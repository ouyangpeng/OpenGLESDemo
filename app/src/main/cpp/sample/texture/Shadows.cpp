//
// Created by OuyangPeng on 2021/10/25.
//

// 第一遍渲染中，我们从光源的角度出发渲染场景，将片段深度值记录在一个纹理中
// 第二遍渲染中，我们从眼睛位置的角度出发渲染场景。
//              在片段着色器中，通过采样深度纹理执行深度测试，确定片段是否在阴影中。
// 此外，我们使用百分比渐进过滤（PCF）技术采样深度纹理，以生成软件阴影。

#include <cstdlib>
#include "Shadows.h"

void Shadows::create() {
    GLUtils::printGLInfo();

    GLfloat *positions;
    GLuint *indices;

    const char *vShadowMapShaderStr = GLUtils::openTextFile(
            "vertex/vertex_shader_shadows_map.glsl");
    const char *fShadowMapShaderStr = GLUtils::openTextFile(
            "fragment/fragment_shader_shadows_map.glsl");

    const char *vSceneShaderStr = GLUtils::openTextFile(
            "vertex/vertex_shader_shadows_scene.glsl");
    const char *fSceneShaderStr = GLUtils::openTextFile(
            "fragment/fragment_shader_shadows_scene.glsl");

    shadowMapProgramObject = GLUtils::createProgram(&vShadowMapShaderStr, &fShadowMapShaderStr);
    sceneProgramObject = GLUtils::createProgram(&vSceneShaderStr, &fSceneShaderStr);

    if (!shadowMapProgramObject || !sceneProgramObject) {
        LOGD("Could not create program")
        return;
    }

    // Get the uniform locations
    sceneMvpLoc = glGetUniformLocation(sceneProgramObject, "u_mvpMatrix");
    sceneMvpLightLoc = glGetUniformLocation(sceneProgramObject, "u_mvpLightMatrix");

    shadowMapMvpLoc = glGetUniformLocation(shadowMapProgramObject, "u_mvpMatrix");
    shadowMapMvpLightLoc = glGetUniformLocation(shadowMapProgramObject, "u_mvpLightMatrix");

    // Get the sampler location
    shadowMapSamplerLoc = glGetUniformLocation(sceneProgramObject, "s_shadowMap");

    // Generate the vertex and index data for the ground
    groundGridSize = 3;
    groundNumIndices = esGenSquareGrid(groundGridSize, &positions, &indices);

    // Index buffer object for the ground model
    glGenBuffers(1, &groundIndicesIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundIndicesIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, groundNumIndices * sizeof(GLuint),
                 indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    free(indices);

    // Position VBO for ground model
    glGenBuffers(1, &groundPositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groundPositionVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 groundGridSize * groundGridSize * sizeof(GLfloat) * 3,
                 positions, GL_STATIC_DRAW);
    free(positions);

    // Generate the vertex and index date for the cube model
    cubeNumIndices = esGenCube(1.0f,
                               &positions,
                               nullptr,
                               nullptr,
                               &indices);



    // Index buffer object for cube model
    glGenBuffers(1, &cubeIndicesIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * cubeNumIndices, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    free(indices);

    // Position VBO for cube model
    glGenBuffers(1, &cubePositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubePositionVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat) * 3, positions, GL_STATIC_DRAW);
    free(positions);


    // setup transformation matrices
    eyePosition[0] = -5.0f;
    eyePosition[1] = 3.0f;
    eyePosition[2] = 5.0f;

    lightPosition[0] = 10.0f;
    lightPosition[1] = 5.0f;
    lightPosition[2] = 2.0f;

    // create depth texture
    if (!initShadowMap()) {
        return;
    }

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // disable culling
    glDisable(GL_CULL_FACE);

    // enable depth test
    glEnable(GL_DEPTH_TEST);

}

//
// Initialize the MVP matrix
//
void Shadows::initMVP() {
    ESMatrix perspective;
    ESMatrix ortho;
    ESMatrix modelView;
    ESMatrix model;
    ESMatrix view;

    float aspect;
    // Compute the window aspect ratio
    aspect = (GLfloat) mWidth / (GLfloat) mHeight;

    // Generate a perspective matrix with a 45 degree FOV for the scene rendering
    esMatrixLoadIdentity(&perspective);
    esPerspective(&perspective, 45.0f, aspect, 0.1f, 100.0f);

    // 用光源位置建立一个MVP矩阵
    // 通过连接 正交投影、模型和视图变换矩阵生成的MVP变换矩阵
    // Generate an orthographic projection matrix for the shadow map rendering
    esMatrixLoadIdentity(&ortho);
    esOrtho(&ortho, -10, 10, -10, 10, -30, 30);

    // GROUND
    // Generate a model view matrix to rotate/translate the ground
    esMatrixLoadIdentity(&model);

    // Center the ground
    esTranslate(&model, -2.0f, -2.0f, 0.0f);
    esScale(&model, 10.0f, 10.0f, 10.0f);
    esRotate(&model, 90.0f, 1.0f, 0.0f, 0.0f);

    // ----------   用眼睛位置建立MVP矩阵  ----------  开始

    // create view matrix transformation from the eye position
    esMatrixLookAt(&view,
                   eyePosition[0], eyePosition[1], eyePosition[2],
                   0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f);

    esMatrixMultiply(&modelView, &model, &view);

    // Compute the final ground MVP for the scene rendering by multiplying the 
    // modelview and perspective matrices together
    esMatrixMultiply(&groundMvpMatrix, &modelView, &perspective);

    // create view matrix transformation from the light position
    esMatrixLookAt(&view,
                   lightPosition[0], lightPosition[1], lightPosition[2],
                   0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f);

    esMatrixMultiply(&modelView, &model, &view);

    // Compute the final ground MVP for the shadow map rendering by multiplying the 
    // modelview and ortho matrices together
    esMatrixMultiply(&groundMvpLightMatrix, &modelView, &ortho);

    // ----------   用眼睛位置建立MVP矩阵  ----------  结束

    // CUBE
    // position the cube
    esMatrixLoadIdentity(&model);
    esTranslate(&model, 5.0f, -0.4f, -3.0f);
    esScale(&model, 1.0f, 2.5f, 1.0f);
    esRotate(&model, -15.0f, 0.0f, 1.0f, 0.0f);

    // create view matrix transformation from the eye position
    esMatrixLookAt(&view,
                   eyePosition[0], eyePosition[1], eyePosition[2],
                   0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f);

    esMatrixMultiply(&modelView, &model, &view);

    // Compute the final cube MVP for scene rendering by multiplying the 
    // modelview and perspective matrices together
    esMatrixMultiply(&cubeMvpMatrix, &modelView, &perspective);

    // create view matrix transformation from the light position
    esMatrixLookAt(&view,
                   lightPosition[0], lightPosition[1], lightPosition[2],
                   0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f);

    esMatrixMultiply(&modelView, &model, &view);

    // Compute the final cube MVP for shadow map rendering by multiplying the 
    // modelview and ortho matrices together
    esMatrixMultiply(&cubeMvpLightMatrix, &modelView, &ortho);
}


// 创建一个1024*1024的16位深度纹理，以保存阴影贴图。
// 这个阴影贴图设置了 GL_LINEAR 纹理过滤。
// 当它与 sampler2Dshadow采样器类型一起使用时，就得到基于硬件的PCF， 硬件将一次执行4个深度比较。
// 然后，我们展示了用深度纹理附着渲染到帧缓冲区对象的方法。

// 创建深度纹理并将其连接到帧缓冲区对象
int Shadows::initShadowMap() {
    GLenum none = GL_NONE;
    GLint defaultFramebuffer = 0;

    // use 1K by 1K texture for shadow map
    shadowMapTextureWidth = shadowMapTextureHeight = 1024;

    glGenTextures(1, &shadowMapTextureId);
    glBindTexture(GL_TEXTURE_2D, shadowMapTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Setup hardware comparison
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                 shadowMapTextureWidth, shadowMapTextureHeight,
                 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);

    // setup fbo
    glGenFramebuffers(1, &shadowMapBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBufferId);

    glDrawBuffers(1, &none);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_TEXTURE_2D, shadowMapTextureId,0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMapTextureId);

    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        return GL_FALSE;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);

    return GL_TRUE;
}

void Shadows::draw() {

    GLint defaultFramebuffer = 0;

    // Initialize matrices
    initMVP();

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);

    // FIRST PASS: Render the scene from light position to generate the shadow map texture
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBufferId);

    // Set the viewport
    glViewport(0, 0, shadowMapTextureWidth, shadowMapTextureHeight);

    // 为了使用这些着色器，清除深度缓冲区并禁用颜色渲染。
    // 可以使用多边形偏移命令增大写入纹理的深度值，以避免因为精度问题而造成的阴影渲染伪像

    // clear depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    // disable color rendering, only write to depth buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // reduce shadow rendering artifact
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(5.0f, 100.0f);


    glUseProgram(shadowMapProgramObject);
    drawScene(shadowMapMvpLoc, shadowMapMvpLightLoc);

    glDisable(GL_POLYGON_OFFSET_FILL);

    // SECOND PASS: Render the scene from eye location using the shadow map texture created in the first pass
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Set the viewport
    glViewport(0, 0, mWidth, mHeight);

    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // Use the scene program object
    glUseProgram(sceneProgramObject);

    // Bind the shadow map texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMapTextureId);

    // Set the sampler texture unit to 0
    glUniform1i(shadowMapSamplerLoc, 0);

    drawScene(sceneMvpLoc, sceneMvpLightLoc);
}

//
// Draw the model
//
void Shadows::drawScene(GLint mvpLoc, GLint mvpLightLoc) {
    // Draw the ground
    // Load the vertex position
    glBindBuffer(GL_ARRAY_BUFFER, groundPositionVBO);
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), (const void *) nullptr);
    glEnableVertexAttribArray(POSITION_LOC);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundIndicesIBO);

    // Load the MVP matrix for the ground model
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (GLfloat *) &groundMvpMatrix.m[0][0]);
    glUniformMatrix4fv(mvpLightLoc, 1, GL_FALSE, (GLfloat *) &groundMvpLightMatrix.m[0][0]);

    // Set the ground color to light gray
    glVertexAttrib4f(COLOR_LOC, 0.9f, 0.9f, 0.9f, 1.0f);

    glDrawElements(GL_TRIANGLES, groundNumIndices, GL_UNSIGNED_INT, (const void *) nullptr);

    // Draw the cube
    // Load the vertex position
    glBindBuffer(GL_ARRAY_BUFFER, cubePositionVBO);
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), (const void *) nullptr);
    glEnableVertexAttribArray(POSITION_LOC);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesIBO);

    // Load the MVP matrix for the cube model
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (GLfloat *) &cubeMvpMatrix.m[0][0]);
    glUniformMatrix4fv(mvpLightLoc, 1, GL_FALSE, (GLfloat *) &cubeMvpLightMatrix.m[0][0]);

    // Set the cube color to red
    glVertexAttrib4f(COLOR_LOC, 1.0f, 0.0f, 0.0f, 1.0f);

    glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_INT, (const void *) nullptr);
}

void Shadows::shutdown() {
    glDeleteBuffers(1, &groundPositionVBO);
    glDeleteBuffers(1, &groundIndicesIBO);

    glDeleteBuffers(1, &cubePositionVBO);
    glDeleteBuffers(1, &cubeIndicesIBO);

    // Delete shadow map
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBufferId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapBufferId);
    glDeleteTextures(1, &shadowMapTextureId);

    // Delete program object
    GLUtils::DeleteProgram(sceneProgramObject);
    GLUtils::DeleteProgram(shadowMapProgramObject);
}
