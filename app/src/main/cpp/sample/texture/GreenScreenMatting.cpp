//
// Created by OuyangPeng on 2022/3/10.
//

#include "GreenScreenMatting.h"

//
// Created by OuyangPeng on 2021/12/10.
//

// 可以参考下面的博客理解代码
// NDK OpenGL ES 3.0 开发（八）：坐标系统
// https://blog.csdn.net/Kennethdroid/article/details/100898155

#include <glm/ext.hpp>
#include "GreenScreenMatting.h"

#define CS_VERTEX_POS_INDX  0
#define CS_TEXTURE_POS_INDX 1

//顶点坐标
static GLfloat cs_vVertices[] = {
        -1.0f, 1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f, -1.0f, 0.0f,  // Position 2
        1.0f, 1.0f, 0.0f,  // Position 3
};

//正常纹理坐标
static GLfloat cs_vTexCoors[] = {
        0.0f, 0.0f,        // TexCoord 0
        0.0f, 1.0f,        // TexCoord 1
        1.0f, 1.0f,        // TexCoord 2
        1.0f, 0.0f         // TexCoord 3
};

// 注意索引从0开始!
static GLushort cs_indices[] = {
        0, 1, 2,        // 第一个三角形
        0, 2, 3         // 第二个三角形
};

GreenScreenMatting::GreenScreenMatting() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

GreenScreenMatting::~GreenScreenMatting() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}


void GreenScreenMatting::Create() {
    // Create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_coord_system.glsl");
    // 片段着色器
//    FRAGMENT_SHADER = GLUtils::openTextFile(
//            "fragment/fragment_shader_green_screen_matting.glsl");

    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_green_screen_matting2.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (m_ProgramObj == GL_NONE) {
        LOGE("GreenScreenMatting::Init m_ProgramObj == GL_NONE")
        return;
    }
    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");

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

void GreenScreenMatting::Draw() {

    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 旋转角度变换，更新变换矩阵
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) m_Width / (float) m_Height);

    // Use the program object
    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void GreenScreenMatting::Shutdown() {
    GLBaseSample::Shutdown();
    glDeleteBuffers(3, m_VboIds);
    glDeleteVertexArrays(1, &m_VaoId);
    glDeleteTextures(1, &m_TextureId);
}

void GreenScreenMatting::LoadImage(NativeImage *pImage) {
    LOGD("GreenScreenMatting::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void
GreenScreenMatting::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    m_AngleX = (int) rotateX;
    m_AngleY = (int) rotateY;
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void GreenScreenMatting::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                        float ratio) const {
    LOGD("GreenScreenMatting::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY,
         ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 mProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 mProjection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 mProjection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    // 参考链接 https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/ 去理解摄像机
    glm::mat4 mView = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 mModel = glm::mat4(1.0f);
    mModel = glm::scale(mModel, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    mModel = glm::rotate(mModel, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    mModel = glm::rotate(mModel, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    mModel = glm::translate(mModel, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = mProjection * mView * mModel;
}
