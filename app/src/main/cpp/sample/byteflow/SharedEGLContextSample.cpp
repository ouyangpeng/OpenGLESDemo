//
// Created by OuyangPeng on 2021/12/24.
// 参考博客：
// OpenGL ES 共享上下文时，可以共享哪些资源？
// https://www.jianshu.com/p/fb346e3eadf7


//共享上下文时可以共享哪些资源
//可以共享的资源：
//  纹理；
//  shader；
//  program 着色器程序；
//  buffer 类对象，如 VBO、 EBO、 RBO 等 。
//
//不可以共享的资源：
//  FBO 帧缓冲区对象（不属于 buffer 类）；
//  VAO 顶点数组对象（不属于 buffer 类）。
// 这里解释下，在不可以共享的资源中，FBO 和 VAO 属于资源管理型对象，
// FBO 负责管理几种缓冲区，本身不占用资源，VAO 负责管理 VBO 或 EBO ，本身也不占用资源。
//

#include "SharedEGLContextSample.h"

#define VERTEX_POS_INDX  0
#define TEXTURE_POS_INDX 1


//顶点坐标
static GLfloat vVertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
};

//正常纹理坐标
static GLfloat vTexCoors[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
};

//fbo 纹理坐标与正常纹理方向不同，原点位于左下角
static GLfloat vFboTexCoors[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
};

static GLushort indices[] = {
        0, 1, 2,
        1, 3, 2
};


SharedEGLContextSample::SharedEGLContextSample() {
    m_VaoId = GL_NONE;
    m_VboIds[0] = GL_NONE;
    m_ImageTextureId = GL_NONE;
    m_FboTextureId = GL_NONE;
    m_FboProgramObj = GL_NONE;
}

SharedEGLContextSample::~SharedEGLContextSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void SharedEGLContextSample::LoadImage(NativeImage *pImage) {
    LOGD("SharedEGLContextSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void SharedEGLContextSample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_texture_map.glsl");

    // 片段着色器脚本
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    // 编译链接用于普通渲染的着色器程序
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    // 用于离屏渲染的片段着色器脚本，作为另一个线程的着色器程序
    const char *fragmentRgb2yuv = GLUtils::openTextFile(
            "fragment/fragment_shader_shared_egl_context.glsl");
    // 编译链接用于离屏渲染的着色器程序
    m_FboProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &fragmentRgb2yuv);

    if (m_ProgramObj == GL_NONE || m_FboProgramObj == GL_NONE) {
        LOGE("RGB2YUVSample::Init Create program fail")
        return;
    }


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
    glGenVertexArrays(1, &m_VaoId);
    // 初始化用于普通渲染的 VAO
    glBindVertexArray(m_VaoId);

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

    //=========================================== GLEnv相关 ===========================================

    // 将主渲染生成的纹理、 program 、VBO 资源以及 EGLContext 传递给新线程。
    SizeF imgSize;
    imgSize.width = m_RenderImage.width;
    imgSize.height = m_RenderImage.height;

    // 直接获取到上下文对象 EGLContext
    m_GLEnv.sharedCtx = eglGetCurrentContext();
    m_GLEnv.program = m_FboProgramObj;
    m_GLEnv.inputTexId = m_ImageTextureId;
    m_GLEnv.vboIds[0] = m_VboIds[0];
    m_GLEnv.vboIds[1] = m_VboIds[2];
    m_GLEnv.vboIds[2] = m_VboIds[3];
    m_GLEnv.imgSize = imgSize;
    //主线程回调函数
    m_GLEnv.renderDone = OnAsyncRenderDone;
    m_GLEnv.callbackCtx = this;
    LOGD("SharedEGLContextSample::Init sharedCtx=%p", m_GLEnv.sharedCtx)

    //将共享的资源发送给新线程
    GLRenderLooper::GetInstance()->postMessage(MSG_SurfaceCreated, &m_GLEnv);

    GLRenderLooper::GetInstance()->postMessage(MSG_SurfaceChanged,
                                               m_RenderImage.width, m_RenderImage.height);
    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// 主线程渲染时，首先向新线程发送渲染指令，然后等待其渲染结束，
// 新线程渲染结束后会调用 OnAsyncRenderDone 函数通知主线程进行上屏渲染。

// 需要注意的是：多线程渲染要确保纹理等共享资源不会被同时访问，否则会导致渲染出错。
void SharedEGLContextSample::Draw() {
    LOGD("SharedEGLContextSample::Draw")
    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    {
        unique_lock<mutex> lock(m_Mutex);
        // 向新线程发送渲染指令，然后等待其渲染结束
        GLRenderLooper::GetInstance()->postMessage(MSG_DrawFrame);
        m_Cond.wait(lock);
    }

    // 主线程进行上屏渲染
    // 普通渲染
    glViewport(0, 0, m_Width, m_Height);
    glUseProgram(m_ProgramObj);
    GO_CHECK_GL_ERROR()

    glBindVertexArray(m_VaoId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    GLUtils::setInt(m_ProgramObj, "s_TextureMap", 0);
    GO_CHECK_GL_ERROR()

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    GO_CHECK_GL_ERROR()

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);
}

void SharedEGLContextSample::OnAsyncRenderDone(void *callback, int fboTexId) {
    LOGD("SharedEGLContextSample::OnAsyncRenderDone callback=%p, fboTexId=%d", callback, fboTexId)
    // 新线程渲染结束后会调用 OnAsyncRenderDone 函数通知主线程进行上屏渲染
    auto *ctx = static_cast<SharedEGLContextSample *>(callback);
    unique_lock<mutex> lock(ctx->m_Mutex);
    ctx->m_FboTextureId = fboTexId;
    ctx->m_Cond.notify_all();
}

void SharedEGLContextSample::Shutdown() {
    GLRenderLooper::GetInstance()->postMessage(MSG_SurfaceDestroyed);
    GLRenderLooper::ReleaseInstance();
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }

    if (m_FboProgramObj) {
        glDeleteProgram(m_FboProgramObj);
    }

    if (m_ImageTextureId) {
        glDeleteTextures(1, &m_ImageTextureId);
    }

    if (m_VboIds[0]) {
        glDeleteBuffers(4, m_VboIds);
    }

    if (m_VaoId) {
        glDeleteVertexArrays(1, &m_VaoId);
    }
}


