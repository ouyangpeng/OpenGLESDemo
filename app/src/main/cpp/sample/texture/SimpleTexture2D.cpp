//
// Created by OuyangPeng on 2021/10/24.
//

#include "SimpleTexture2D.h"

void SimpleTexture2D::Create() {
    GLUtils::printGLInfo();

    // Main Program
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_simple_texture_2d.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_simple_texture_2d.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }

    // Get the sampler location
    samplerLoc = glGetUniformLocation(m_ProgramObj, "s_texture");

    // Load the texture  加载纹理
    textureId = CreateSimpleTexture2D();

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}


void SimpleTexture2D::Draw() {
    GLfloat vVertices[] =
            {
                    -0.5f, 0.5f, 0.0f,     // Position 0
                    0.0f, 0.0f,            // TexCoord 0

                    -0.5f, -0.5f, 0.0f,     // Position 1
                    0.0f, 1.0f,            // TexCoord 1

                    0.5f, -0.5f, 0.0f,      // Position 2
                    1.0f, 1.0f,            // TexCoord 2

                    0.5f, 0.5f, 0.0f,      // Position 3
                    1.0f, 0.0f             // TexCoord 3
            };

    // 注意索引从0开始!
    GLushort indices[] = {
            0, 1, 2,		// 第一个三角形
            0, 2, 3			// 第二个三角形
    };

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(m_ProgramObj);

    // Load the vertext position
    glVertexAttribPointer(SIMPLE_TEXTURE_2D_VERTEX_POS_INDEX, 3, GL_FLOAT,
                          GL_FALSE, 5 * sizeof(GLfloat), vVertices);

    // Load the texture coordinate
    glVertexAttribPointer(SIMPLE_TEXTURE_2D_VERTEX_TEXTCOORD_INDEX, 2, GL_FLOAT,
                          GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

    glEnableVertexAttribArray(SIMPLE_TEXTURE_2D_VERTEX_POS_INDEX);
    glEnableVertexAttribArray(SIMPLE_TEXTURE_2D_VERTEX_TEXTCOORD_INDEX);

    // Bind the texture
    // 激活当前纹理单元，以便后续的glBindTexture调用将纹理绑定到当前活动单元
    glActiveTexture(GL_TEXTURE0);
    // 纹理绑定到纹理单元0
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set the sampler texture unit to 0
    // 采样器设置为纹理单元0
    glUniform1i(samplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

// Create a simple 2x2 texture image with four different colors
GLuint SimpleTexture2D::CreateSimpleTexture2D() {
    //Texture object handle
    GLuint simple_textureId;

    // 2x2 Image, 3 bytes per pixel (R,G,B)
    GLubyte pixels[4 * 3] =
            {
                    255, 0, 0, // Red
                    0, 255, 0, // Green
                    0, 0, 255, // Blue
                    255, 255, 0  // Yellow
            };

    // Use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate a texture object
    glGenTextures(1, &simple_textureId);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, simple_textureId);

    // Load the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 2, 2, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set the filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return simple_textureId;
}

void SimpleTexture2D::Shutdown() {
    // Delete texture object
    glDeleteTextures ( 1, &textureId );

    // Delete program object
    GLUtils::DeleteProgram (m_ProgramObj );
}
