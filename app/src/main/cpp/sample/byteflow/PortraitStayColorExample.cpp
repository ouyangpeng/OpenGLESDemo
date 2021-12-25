//
// Created by OuyangPeng on 2021/12/25 0025.
// 参考博客
// 人像抠图 + OpenGL ES 还能这样玩？没想到吧
// https://www.jianshu.com/p/a85f94e0450a
//

#include "PortraitStayColorExample.h"

static GLfloat verticesCoords[] = {
        -1.0f, 1.0f, 0.0f,   // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f, -1.0f, 0.0f,   // Position 2
        1.0f, 1.0f, 0.0f,    // Position 3
};

static GLfloat textureCoords[] = {
        0.0f, 0.0f,        // TexCoord 0
        0.0f, 1.0f,        // TexCoord 1
        1.0f, 1.0f,        // TexCoord 2
        1.0f, 0.0f         // TexCoord 3
};

static GLushort indices[] = {0, 1, 2, 0, 2, 3};

PortraitStayColorExample::PortraitStayColorExample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_frameIndex = 0;
}

PortraitStayColorExample::~PortraitStayColorExample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
    NativeImageUtil::FreeNativeImage(&m_GrayImage);
    NativeImageUtil::FreeNativeImage(&m_MappingImage);
}

void PortraitStayColorExample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_coord_system.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_stay_color.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (m_ProgramObj == GL_NONE) {
        LOGD("PortraitStayColorExample::Init create program fail")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "u_texture0");
    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");

    //=========================================== VBO和VAO相关===========================================
    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);

    //===========================================  字节对齐相关===========================================

    // 另外需要注意 OpenGL 访问的图像内存默认是 4 字节对齐，
    // 这样灰度 Mask 图的宽度不是 4 的整数倍的话，会有花屏现象，这里需要取消对齐设置
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //=========================================== 纹理相关===========================================
    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glGenTextures(1, &m_GrayTexId);
    glBindTexture(GL_TEXTURE_2D, m_GrayTexId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glGenTextures(1, &m_MappingTexId);
    glBindTexture(GL_TEXTURE_2D, m_MappingTexId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //upload RGBA image data
    // 纹理的基本格式
    //格式                        纹素数据描述
    //GL_RED                        (红)
    //GL_RG                         (红， 绿)
    //GL_RGB                        (红， 绿， 蓝)
    //GL_RGBA                       (红， 绿， 蓝， Alpha)
    //GL_LUMINANCE                  (亮度)
    //GL_LUMINANCE_ALPHA            (亮度，Alpha)
    //GL_ALPHA                      (Alpha)
    //GL_DEPTH_COMPONENT            (深度)
    //GL_DEPTH_STENCIL             (深度， 模板)
    //GL_RED_INTEGER               (整数红)
    //GL_RG_INTEGER                (整数红， 整数绿)
    //GL_RGB_INTEGER               (整数红， 整数绿， 整数蓝)
    //GL_RGBA_INTEGER              (整数红， 整数绿， 整数蓝， Alpha)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    // GL_RGBA  红， 绿， 蓝， Alpha
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_GrayTexId);
    // GL_LUMINANCE  亮度
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_GrayImage.width, m_GrayImage.height,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_GrayImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_MappingTexId);
    // GL_RGBA  红， 绿， 蓝， Alpha
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_MappingImage.width, m_MappingImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_MappingImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void PortraitStayColorExample::Draw() {
    LOGD("PortraitStayColorExample::Draw()")

    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) m_Width / (float) m_Height);

    // Use the program object
    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    GLUtils::setInt(m_ProgramObj, "u_texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_GrayTexId);
    GLUtils::setInt(m_ProgramObj, "u_texture1", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_MappingTexId);
    GLUtils::setInt(m_ProgramObj, "u_texture2", 2);

    GLUtils::setVec2(m_ProgramObj, "u_texSize",
                     (float) m_RenderImage.width, (float) m_RenderImage.height);

    m_frameIndex++;
    float offset = (float) (sin(m_frameIndex * MATH_PI / 160) + 1.0f) / 2.0f;
    GLUtils::setFloat(m_ProgramObj, "u_offset", offset);
    GLUtils::setFloat(m_ProgramObj, "u_time", (float) m_frameIndex * 0.04f);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void PortraitStayColorExample::Shutdown() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
        glDeleteTextures(1, &m_GrayTexId);
        glDeleteTextures(1, &m_MappingTexId);
    }
}

void PortraitStayColorExample::LoadImage(NativeImage *pImage) {
    LOGD("PortraitStayColorExample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void PortraitStayColorExample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGD("PortraitStayColorExample::LoadMultiImageWithIndex pImage = %p,[w=%d,h=%d,f=%d]",
         pImage->ppPlane[0], pImage->width, pImage->height, pImage->format)
    if (pImage) {
        switch (index) {
            case 0:
                m_GrayImage.width = pImage->width;
                m_GrayImage.height = pImage->height;
                m_GrayImage.format = pImage->format;
                NativeImageUtil::CopyNativeImage(pImage, &m_GrayImage);
                break;
            case 1:
                m_MappingImage.width = pImage->width;
                m_MappingImage.height = pImage->height;
                m_MappingImage.format = pImage->format;
                NativeImageUtil::CopyNativeImage(pImage, &m_MappingImage);
                break;
            default:
                break;
        }
    }
}

void PortraitStayColorExample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                                     float scaleY) {
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void PortraitStayColorExample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                               float ratio) const {
    LOGD("PortraitStayColorExample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f",
         angleX, angleY, ratio)

    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

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
