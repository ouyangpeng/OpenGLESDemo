//
// Created by OuyangPeng on 2021/12/23.
// 参考博客
// 使用 OpenGL 实现 RGB 到 YUV 的图像格式转换
// https://blog.csdn.net/Kennethdroid/article/details/117675581
//

#include "RGB2YUVSample.h"

#define VERTEX_POS_INDX  0
#define TEXTURE_POS_INDX 1


//顶点坐标
static GLfloat vVertices[] = {
        -1.0f, -1.0f, 0.0f, // Position 0
        1.0f, -1.0f, 0.0f,  // Position 1
        -1.0f, 1.0f, 0.0f,  // Position 2
        1.0f, 1.0f, 0.0f,   // Position 3
};

//正常纹理坐标
static GLfloat vTexCoors[] = {
        0.0f, 1.0f,         // TexCoord 0
        1.0f, 1.0f,         // TexCoord 1
        0.0f, 0.0f,         // TexCoord 2
        1.0f, 0.0f,         // TexCoord 3
};

//fbo 纹理坐标与正常纹理方向不同，原点位于左下角
static GLfloat vFboTexCoors[] = {
        0.0f, 0.0f,        // TexCoord 0
        1.0f, 0.0f,        // TexCoord 1
        0.0f, 1.0f,        // TexCoord 2
        1.0f, 1.0f,        // TexCoord 3
};

static GLushort indices[] = {
        0, 1, 2,    // 第一个三角形
        1, 3, 2     // 第二个三角形
};

RGB2YUVSample::RGB2YUVSample() {
    m_ImageTextureId = GL_NONE;
    m_FboTextureId = GL_NONE;
    m_SamplerLoc = GL_NONE;
    m_FboId = GL_NONE;
    m_FboProgramObj = GL_NONE;
    m_FboVertexShader = GL_NONE;
    m_FboFragmentShader = GL_NONE;
    m_FboSamplerLoc = GL_NONE;
}

RGB2YUVSample::~RGB2YUVSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void RGB2YUVSample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_texture_map.glsl");

    // 片段着色器脚本
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    const char *fragmentRgb2yuv = GLUtils::openTextFile(
            "fragment/fragment_shader_rgb2yuv.glsl");
    m_FboProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &fragmentRgb2yuv);

    if (m_ProgramObj == GL_NONE || m_FboProgramObj == GL_NONE) {
        LOGE("RGB2YUVSample::Init Create program fail")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    m_FboSamplerLoc = glGetUniformLocation(m_FboProgramObj, "s_TextureMap");

    //=========================================== VBO和VAO相关===========================================
    // 生成 VBO ，加载顶点数据和索引数据
    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(4, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoors), vTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vFboTexCoors), vFboTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GO_CHECK_GL_ERROR()

    // 生成 2 个 VAO，一个用于普通渲染，另一个用于离屏渲染
    // Generate VAO Ids
    glGenVertexArrays(2, m_VaoIds);

    // 初始化用于普通渲染的 VAO
    // Normal rendering VAO
    glBindVertexArray(m_VaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(TEXTURE_POS_INDX);
    glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(GL_NONE);


    // 初始化用于离屏渲染的 VAO
    // FBO off screen rendering VAO
    glBindVertexArray(m_VaoIds[1]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glEnableVertexAttribArray(TEXTURE_POS_INDX);
    glVertexAttribPointer(TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(GL_NONE);

    //=========================================== 纹理相关===========================================
    // 创建并初始化图像纹理
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR()

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    if (!CreateFrameBufferObj()) {
        LOGD("RGB2YUVSample::Init CreateFrameBufferObj fail")
        return;
    }
}

void RGB2YUVSample::Draw() {
    // 离屏渲染
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Do FBO off screen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);

    // 渲染成 yuyv 宽度像素减半,glviewport 宽度减半
    glViewport(0, 0, m_RenderImage.width / 2, m_RenderImage.height);

    glUseProgram(m_FboProgramObj);
    glBindVertexArray(m_VaoIds[1]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_FboSamplerLoc, 0);

    //参考原理图，偏移量应该设置为 1/(width / 2) * 1/2 = 1 / width; 理论上纹素的一半
    auto texelOffset = (float) (1.f / (float) m_RenderImage.width);
    GLUtils::setFloat(m_FboProgramObj, "u_Offset", texelOffset);
    GO_CHECK_GL_ERROR()

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //YUYV buffer = width * height * 2;  转换成 YUYV 时数据量减半，注意 buffer
    auto *pBuffer = new uint8_t[m_RenderImage.width * m_RenderImage.height * 2];

    NativeImage nativeImage = m_RenderImage;
    nativeImage.format = IMAGE_FORMAT_YUYV;
    nativeImage.ppPlane[0] = pBuffer;
    FUN_BEGIN_TIME("FBO glReadPixels")
        glReadPixels(0, 0, m_RenderImage.width / 2, nativeImage.height,
                     GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
    FUN_END_TIME("FBO cost glReadPixels")

    // 下面目录如果不存在，则不会生成
    std::string path(DEFAULT_OGL_ASSETS_DIR);
    NativeImageUtil::DumpNativeImage(&nativeImage, path.c_str(), "RGB2YUV");
    // 直接保存到相册目录下
    //NativeImageUtil::DumpNativeImage(&nativeImage, "/sdcard/DCIM","RGB2YUV");
    delete[]pBuffer;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 普通渲染
    // Do normal rendering
    glViewport(0, 0, m_Width, m_Height);
    glUseProgram(m_ProgramObj);
    GO_CHECK_GL_ERROR()

    glBindVertexArray(m_VaoIds[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_SamplerLoc, 0);
    GO_CHECK_GL_ERROR()

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    GO_CHECK_GL_ERROR()

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);
}

void RGB2YUVSample::Shutdown() {
    GLBaseSample::Shutdown();

    if (m_FboProgramObj) {
        glDeleteProgram(m_FboProgramObj);
    }

    if (m_ImageTextureId) {
        glDeleteTextures(1, &m_ImageTextureId);
    }

    if (m_FboTextureId) {
        glDeleteTextures(1, &m_FboTextureId);
    }

    if (m_VboIds[0]) {
        glDeleteBuffers(4, m_VboIds);
    }

    if (m_VaoIds[0]) {
        glDeleteVertexArrays(2, m_VaoIds);
    }

    if (m_FboId) {
        glDeleteFramebuffers(1, &m_FboId);
    }

}

void RGB2YUVSample::LoadImage(NativeImage *pImage) {
    LOGD("RGB2YUVSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

bool RGB2YUVSample::CreateFrameBufferObj() {
    // 创建并初始化 FBO 纹理
    glGenTextures(1, &m_FboTextureId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 创建并初始化 FBO
    glGenFramebuffers(1, &m_FboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);

    // 我们在 shader 中执行两次采样，RGBA 像素（R0,G0,B0,A0）转换为（Y0，U0，V0），像素（R1,G1,B1,A1）转换为（Y1），
    // 然后组合成（Y0，U0，Y1，V0），这样 8 个字节表示的 2 个 RGBA 像素就转换为 4 个字节表示的 2 个 YUYV 像素。
    // 转换成 YUYV 时数据量减半，那么 glViewPort 时 width 变为原来的一半，同样 glReadPixels 时 width 也变为原来的一半。
    // 实现 RGBA 转成 YUYV 要保证原图分辨率不变，建议使用 FBO 离屏渲染 ，
    // 这里注意绑定给 FBO 的纹理是用来容纳 YUYV 数据，其宽度应该设置为原图的一半。

    //FBO 纹理是用来容纳 YUYV 数据，其宽度应该设置为原图的一半
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width / 2, m_RenderImage.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGD("RGB2YUVSample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE")
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return true;
}
