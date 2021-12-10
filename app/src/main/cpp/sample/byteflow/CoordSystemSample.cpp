//
// Created by OuyangPeng on 2021/12/10.
//

#include <glm/ext.hpp>
#include "CoordSystemSample.h"

CoordSystemSample::CoordSystemSample() {

    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

CoordSystemSample::~CoordSystemSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}


void CoordSystemSample::create() {
    if (mProgram) return;

    // create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_coord_system.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (mProgram == GL_NONE) {
        LOGE("CoordSystemSample::Init mProgram == GL_NONE")
        return;
    }
    m_MVPMatLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");
    m_SamplerLoc = glGetUniformLocation(mProgram, "s_TextureMap");

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cs_vVertices), cs_vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cs_vTexCoors), cs_vTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cs_indices), cs_indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(CS_VERTEX_POS_INDX);
    glVertexAttribPointer(CS_VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(CS_TEXTURE_POS_INDX);
    glVertexAttribPointer(CS_TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);


    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void CoordSystemSample::draw() {

    if (mProgram == GL_NONE || m_TextureId == GL_NONE) return;

    // Clear the color buffer
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / mHeight);

    // Use the program object
    glUseProgram(mProgram);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void CoordSystemSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
    }
}

void CoordSystemSample::LoadImage(NativeImage *pImage) {
    LOGD("CoordSystemSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void
CoordSystemSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    m_AngleX = rotateX;
    m_AngleY = rotateY;
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void CoordSystemSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGD("CoordSystemSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY,
         ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = Projection * View * Model;
}




