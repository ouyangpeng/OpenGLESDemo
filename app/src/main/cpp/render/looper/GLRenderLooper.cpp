#include <GLUtils.h>
#include <EglCore.h>
#include <OffscreenSurface.h>
#include "GLRenderLooper.h"

mutex GLRenderLooper::m_Mutex;
GLRenderLooper *GLRenderLooper::m_Instance = nullptr;

void GLRenderLooper::handleMessage(LooperMessage *msg) {
    Looper::handleMessage(msg);
    switch (msg->what) {
        case MSG_SurfaceCreated:
            LOGD("GLRenderLooper::handleMessage MSG_SurfaceCreated")
            m_GLEnv = (GLEnv *) msg->obj;
            OnSurfaceCreated();
            break;
        case MSG_SurfaceChanged:
            LOGD("GLRenderLooper::handleMessage MSG_SurfaceChanged")
            OnSurfaceChanged(msg->arg1, msg->arg2);
            break;
        case MSG_DrawFrame:
            LOGD("GLRenderLooper::handleMessage MSG_DrawFrame")
            OnDrawFrame();
            break;
        case MSG_SurfaceDestroyed:
            LOGD("GLRenderLooper::handleMessage MSG_SurfaceDestroyed")
            OnSurfaceDestroyed();
            break;
        default:
            break;
    }
}

GLRenderLooper *GLRenderLooper::GetInstance() {
    LOGD("GLRenderLooper::GetInstance")
    if (m_Instance == nullptr) {
        unique_lock<mutex> lock(m_Mutex);
        if (m_Instance == nullptr) {
            m_Instance = new GLRenderLooper();
        }
    }

    return m_Instance;
}

void GLRenderLooper::ReleaseInstance() {
    LOGD("GLRenderLooper::GetInstance")
    if (m_Instance != nullptr) {
        unique_lock<mutex> lock(m_Mutex);
        if (m_Instance != nullptr) {
            delete m_Instance;
            m_Instance = nullptr;
        }
    }
}

// 在函数 GLRenderLooper::OnSurfaceCreated 中，利用 sharedContext 创建 OpenGL 渲染环境。
void GLRenderLooper::OnSurfaceCreated() {
    LOGD("GLRenderLooper::OnSurfaceCreated")
    //利用 sharedContext 创建 OpenGL 离屏渲染环境
    m_EglCore = new EglCore(m_GLEnv->sharedCtx, FLAG_RECORDABLE);
    SizeF imgSizeF = m_GLEnv->imgSize;
    m_OffscreenSurface = new OffscreenSurface(m_EglCore, imgSizeF.width, imgSizeF.height);
    m_OffscreenSurface->makeCurrent();

    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_GLEnv->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_GLEnv->vboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLEnv->vboIds[2]);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(GL_NONE);

    if (!CreateFrameBufferObj()) {
        LOGD("GLRenderLooper::OnSurfaceCreated CreateFrameBufferObj fail")
    }
}

void GLRenderLooper::OnSurfaceChanged(int w, int h) {
    LOGD("GLRenderLooper::OnSurfaceChanged [w,h]=[%d, %d]", w, h)
}

// GLRenderLooper::OnDrawFrame 函数中，绘制完成注意交换缓冲区，
// 然后将保存绘制结果的纹理，通过回调函数传递给主线程进行上屏渲染。
void GLRenderLooper::OnDrawFrame() {
    LOGD("GLRenderLooper::OnDrawFrame")
    SizeF imgSizeF = m_GLEnv->imgSize;

    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glViewport(0, 0, imgSizeF.width, imgSizeF.height);

    glUseProgram(m_GLEnv->program);
    glBindVertexArray(m_VaoId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_GLEnv->inputTexId);
    GLUtils::setInt(m_GLEnv->program, "s_TextureMap", 0);

    float offset = (sin(m_FrameIndex * MATH_PI / 80) + 1.0f) / 2.0f;
    GLUtils::setFloat(m_GLEnv->program, "u_Offset", offset);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //注意交换缓冲区  交换到前台显示
    m_OffscreenSurface->swapBuffers();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //将保存绘制结果的纹理 m_FboTextureId 传递给主线程进行上屏渲染
    m_GLEnv->renderDone(m_GLEnv->callbackCtx, m_FboTextureId);
    m_FrameIndex++;
}

void GLRenderLooper::OnSurfaceDestroyed() {
    LOGD("GLRenderLooper::OnSurfaceDestroyed")
    m_GLEnv->renderDone(m_GLEnv->callbackCtx, m_FboTextureId);
    if (m_VaoId) {
        glDeleteVertexArrays(1, &m_VaoId);
    }
    if (m_FboId) {
        glDeleteFramebuffers(1, &m_FboId);
    }
    if (m_FboTextureId) {
        glDeleteTextures(1, &m_FboTextureId);
    }
    if (m_OffscreenSurface) {
        m_OffscreenSurface->release();
        delete m_OffscreenSurface;
        m_OffscreenSurface = nullptr;
    }
    if (m_EglCore) {
        m_EglCore->release();
        delete m_EglCore;
        m_EglCore = nullptr;
    }
}

bool GLRenderLooper::CreateFrameBufferObj() {
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_GLEnv->imgSize.width, m_GLEnv->imgSize.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGD("RGB2YUVSample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE")
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return true;
}


