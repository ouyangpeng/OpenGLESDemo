//
// Created by ByteFlow on 2019/7/17.
//

//使用EGL的绘图的一般步骤：
//获取 EGLDisplay 对象：eglGetDisplay()
//初始化与 EGLDisplay 之间的连接：eglInitialize()
//获取 EGLConfig 对象：eglChooseConfig()
//创建 EGLContext 实例：eglCreateContext()
//创建 EGLSurface 实例：eglCreateWindowSurface()
//连接 EGLContext 和 EGLSurface：eglMakeCurrent()
//使用 OpenGL ES API 绘制图形：gl_*()
//切换 front buffer 和 back buffer 送显：eglSwapBuffer()
//断开并释放与 EGLSurface 关联的 EGLContext 对象：eglRelease()
//删除 EGLSurface 对象
//删除 EGLContext 对象
//终止与 EGLDisplay 之间的连接
//————————————————
//版权声明：本文为CSDN博主「欧阳鹏」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/ouyang_peng/article/details/121241714

// 部分效果参考：
// https://www.jianshu.com/p/e4a8c83cd373
// https://www.jianshu.com/p/b1f9572b01a1
// https://www.jianshu.com/p/0eec2727fc2f
// https://www.jianshu.com/p/87866b7b46d9
// https://www.cnblogs.com/zhangzhang-y/p/13499360.html

// 分屏效果原理相关链接
// https://www.cnblogs.com/zhangzhang-y/p/13496126.html
// https://juejin.cn/post/6859934701932118024
// https://juejin.cn/post/7003867568851124238
// https://blog.csdn.net/lin1109221208/article/details/107900718

#include "EGLRender.h"

EGLRender *EGLRender::m_Instance = nullptr;

EGLRender::EGLRender() {
    m_ImageTextureId = GL_NONE;
    m_FboTextureId = GL_NONE;
    m_SamplerLoc = GL_NONE;
    m_TexSizeLoc = GL_NONE;
    m_FboId = GL_NONE;
    m_ProgramObj = GL_NONE;

    m_IsGLContextReady = false;
    m_ShaderIndex = 0;
}

EGLRender::~EGLRender() {
    if (m_RenderImage.ppPlane[0]) {
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
        m_RenderImage.ppPlane[0] = nullptr;
    }

}

void EGLRender::Init(JNIEnv *env, jobject assetManager) {
    LOGD("EGLRender::Init")

    // 初始化设置assetManager  一定要记得初始化，否则会报空指针异常
    GLUtils::setEnvAndAssetManager(env, assetManager);

    if (CreateGlesEnv() == 0) {
        m_IsGLContextReady = true;
    }

    if (!m_IsGLContextReady) return;

    // 编译链接用于普通渲染的着色器程序
    vertexShader = GLUtils::openTextFile(
            "vertex/vertex_shader_texture_map.glsl");

    // 用于普通渲染的片段着色器脚本，简单纹理映射
    m_fShaderStrs[0] = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    // 马赛克
    m_fShaderStrs[1] =  GLUtils::openTextFile(
            "fragment/fragment_shader_egl_mosaic.glsl");
    // 马赛克2
    m_fShaderStrs[2] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_mosaic2.glsl");
    // 马赛克3
    m_fShaderStrs[3] =GLUtils::openTextFile(
            "fragment/fragment_shader_egl_mosaic3.glsl");
    // 马赛克4
    m_fShaderStrs[4] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_mosaic4.glsl");
    // 网格
    m_fShaderStrs[5] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_grid.glsl");
    // 旋转
    m_fShaderStrs[6] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_rotate.glsl");
    // 边缘
    m_fShaderStrs[7] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_edge.glsl");
    // 放大
    m_fShaderStrs[8] =  GLUtils::openTextFile(
            "fragment/fragment_shader_egl_enlarge.glsl");
    // 型变
    m_fShaderStrs[9] =  GLUtils::openTextFile(
            "fragment/fragment_shader_egl_reshape.glsl");
    // 型变2
    m_fShaderStrs[10] =  GLUtils::openTextFile(
            "fragment/fragment_shader_egl_reshape2.glsl");
    // 灰度
    m_fShaderStrs[11] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_grey.glsl");
    // 上下颠倒
    m_fShaderStrs[12] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_upside_down.glsl");
    // 浮雕效果
    m_fShaderStrs[13] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_relief.glsl");
    // 暖色
    m_fShaderStrs[14] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_warm_color.glsl");
    // 冷色
    m_fShaderStrs[15] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_cool_color.glsl");

    // 2分屏
    m_fShaderStrs[16] =  GLUtils::openTextFile(
            "fragment/fragment_shader_egl_split_screen_2.glsl");
    // 3分屏
    m_fShaderStrs[17] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_split_screen_3.glsl");
    // 4分屏
    m_fShaderStrs[18] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_split_screen_4.glsl");
    // 6分屏
    m_fShaderStrs[19] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_split_screen_6.glsl");
    // 9分屏
    m_fShaderStrs[20] =  GLUtils::openTextFile(
            "fragment/fragment_shader_egl_split_screen_9.glsl");
    // 16分屏
    m_fShaderStrs[21] =  GLUtils::openTextFile(
            "fragment/fragment_shader_egl_split_screen_16.glsl");
    // 素描
    m_fShaderStrs[22] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_sketch.glsl");
    // 怀旧
    m_fShaderStrs[23] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_nostalgic.glsl");
    // 流年
    m_fShaderStrs[24] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_fleeting_time.glsl");

    // 动态网格
    m_fShaderStrs[25] = GLUtils::openTextFile(
            "fragment/fragment_shader_egl_dynamic_grid.glsl");


    // 创建并初始化图像纹理
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 创建并初始化 FBO 纹理
    glGenTextures(1, &m_FboTextureId);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    m_ProgramObj = GLUtils::createProgram(&vertexShader, &m_fShaderStrs[m_ShaderIndex]);
    if (!m_ProgramObj) {
        GLUtils::checkGlError("Create Program");
        LOGD("EGLRender::Init Could not create program.")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    m_TexSizeLoc = glGetUniformLocation(m_ProgramObj, "u_texSize");

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(egl_vVertices), egl_vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(egl_vTexCoors), egl_vTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(egl_indices), egl_indices, GL_STATIC_DRAW);

    GO_CHECK_GL_ERROR()

    // Generate VAO Ids
    glGenVertexArrays(1, m_VaoIds);

    // FBO off screen rendering VAO
    glBindVertexArray(m_VaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(EGL_VERTEX_POS_LOC);
    glVertexAttribPointer(EGL_VERTEX_POS_LOC, EGL_VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE,
                          EGL_VERTEX_POS_SIZE * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(EGL_TEXTURE_POS_LOC);
    glVertexAttribPointer(EGL_TEXTURE_POS_LOC, EGL_TEXTURE_POS_SIZE, GL_FLOAT, GL_FALSE,
                          EGL_TEXTURE_POS_SIZE * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(GL_NONE);
}

int EGLRender::CreateGlesEnv() {
    // EGL config attributes
    const EGLint confAttr[] =
            {
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
                    EGL_SURFACE_TYPE,
                    EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
                    EGL_RED_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,// if you need the alpha channel
                    EGL_DEPTH_SIZE, 16,// if you need the depth buffer
                    EGL_STENCIL_SIZE, 8,
                    EGL_NONE
            };

    // EGL context attributes
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };

    // surface attributes
    // the surface size is set to the input frame size
    const EGLint surfaceAttr[] = {
            EGL_WIDTH, 1,
            EGL_HEIGHT, 1,
            EGL_NONE
    };
    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;

    int resultCode = 0;
    do {
        //1. 获取 EGLDisplay 对象，建立与本地窗口系统的连接
        // displayId 指定显示连接，一般使用默认的 EGL_DEFAULT_DISPLAY，即返回与默认原生窗口的连接。
        m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        // EGL_NO_DISPLAY ：连接不可用
        if (m_eglDisplay == EGL_NO_DISPLAY) {
            //Unable to open connection to local windowing system
            LOGD("EGLRender::CreateGlesEnv Unable to open connection to local windowing system")
            resultCode = -1;
            break;
        }

        //2. 初始化 EGL 方法   这个函数初始化EGL内部数据结构，返回EGL实现的主版本号和此版本号。
        if (!eglInitialize(m_eglDisplay, &eglMajVers, &eglMinVers)) {
            // Unable to initialize EGL. Handle and recover
            LOGD("EGLRender::CreateGlesEnv Unable to initialize EGL")
            resultCode = -1;
            break;
        }

        LOGD("EGLRender::CreateGlesEnv EGL init with version %d.%d", eglMajVers, eglMinVers)

        //3. 获取 EGLConfig 对象，确定渲染表面的配置信息
        if (!eglChooseConfig(m_eglDisplay, confAttr, &m_eglConf, 1, &numConfigs)) {
            LOGD("EGLRender::CreateGlesEnv some config is wrong")
            resultCode = -1;
            break;
        }

        //4. 创建渲染表面 EGLSurface, 使用 eglCreatePbufferSurface 创建屏幕外渲染区域
        // 创建屏幕外渲染区域：EGL Pbuffer
        // 除了可以用OpenGL ES 3.0 在屏幕上的窗口渲染之外，还可以渲染称作Pbuffer(像素缓冲区 Pixel buffer的简写）的不可见的屏幕外表面。
        // 和窗口一样，pbuffer可以利用OpenGL ES 3.0 中的任何硬件加速。Pbuffer最常用于生成纹理贴图。
        // 如果你想要做的是渲染到一个纹理，那么我们建议使用帧缓冲区代替Pbuffer，因为 帧缓冲区更高效。
        m_eglSurface = eglCreatePbufferSurface(m_eglDisplay, m_eglConf, surfaceAttr);
        if (m_eglSurface == EGL_NO_SURFACE) {
            switch (eglGetError()) {
                case EGL_BAD_ALLOC:
                    // Not enough resources available. Handle and recover
                    LOGD("EGLRender::CreateGlesEnv Not enough resources available")
                    break;
                case EGL_BAD_CONFIG:
                    // Verify that provided EGLConfig is valid
                    LOGD("EGLRender::CreateGlesEnv provided EGLConfig is invalid")
                    break;
                case EGL_BAD_PARAMETER:
                    // Verify that the EGL_WIDTH and EGL_HEIGHT are
                    // non-negative values
                    LOGD("EGLRender::CreateGlesEnv provided EGL_WIDTH and EGL_HEIGHT is invalid")
                    break;
                case EGL_BAD_MATCH:
                    // Check window and EGLConfig attributes to determine
                    // compatibility and pbuffer-texture parameters
                    LOGD("EGLRender::CreateGlesEnv Check window and EGLConfig attributes")
                    break;
            }
        }

        //5. 创建渲染上下文 EGLContext
        // OpenGL ES的pipeline从程序的角度看就是一个状态机，
        // 有当前的颜色、纹理坐标、变换矩阵、绚染模式等一大堆状态，
        // 这些状态作用于OpenGL API程序提交的顶点坐标等图元从而形成帧缓冲内的像素。
        // 在OpenGL的编程接口中，Context就代表这个状态机，
        // OpenGL API程序的主要工作就是向Context提供图元、设置状态，偶尔也从Context里获取一些信息。
        m_eglCtx = eglCreateContext(m_eglDisplay, m_eglConf, EGL_NO_CONTEXT, ctxAttr);
        if (m_eglCtx == EGL_NO_CONTEXT) {
            EGLint error = eglGetError();
            if (error == EGL_BAD_CONFIG) {
                // Handle error and recover
                LOGD("EGLRender::CreateGlesEnv EGL_BAD_CONFIG")
                resultCode = -1;
                break;
            }
        }

        //6. 绑定上下文
        if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglCtx)) {
            LOGD("EGLRender::CreateGlesEnv MakeCurrent failed")
            resultCode = -1;
            break;
        }
        LOGD("EGLRender::CreateGlesEnv initialize success!")
    } while (false);

    if (resultCode != 0) {
        LOGD("EGLRender::CreateGlesEnv fail")
    }

    return resultCode;
}

void EGLRender::SetImageData(uint8_t *pData, int width, int height) {
    LOGD("EGLRender::SetImageData pData = %p, [w,h] = [%d, %d]", pData, width, height)

    if (pData && m_IsGLContextReady) {
        if (m_RenderImage.ppPlane[0]) {
            NativeImageUtil::FreeNativeImage(&m_RenderImage);
            m_RenderImage.ppPlane[0] = nullptr;
        }

        m_RenderImage.width = width;
        m_RenderImage.height = height;
        m_RenderImage.format = IMAGE_FORMAT_RGBA;
        NativeImageUtil::AllocNativeImage(&m_RenderImage);
        memcpy(m_RenderImage.ppPlane[0], pData, static_cast<size_t>(width * height * 4));

        glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        if (m_FboId == GL_NONE) {
            // 创建并初始化 FBO
            glGenFramebuffers(1, &m_FboId);
            // 绑定 FBO  : 设置当前帧缓冲区对象
            glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
            // 绑定 FBO 纹理
            glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
            // 将纹理连接到FBO 附着 : 将一个2D纹理的某个mip级别或者立方图面连接到帧缓冲区附着点
            /*
             * glFrameBufferTexture2D有以下的参数：
                    target：帧缓冲的目标（绘制、读取或者两者皆有）
                    attachment：我们想要附加的附件类型。当前我们正在附加一个颜色附件。注意最后的0意味着我们可以附加多个颜色附件。我们将在之后的教程中提到。
                    textarget：你希望附加的纹理类型
                    texture：要附加的纹理本身
                    level：多级渐远纹理的级别。我们将它保留为0。
             */
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   m_FboTextureId, 0);
            // 分配内存大小
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            // 检查帧缓冲FBO是否是完整的，如果不是，我们将打印错误信息。
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOGD("EGLRender::SetImageData glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE")
            }
            // 解绑纹理
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
            // 解绑 FBO   记得要解绑帧缓冲，保证我们不会不小心渲染到错误的帧缓冲上。
            glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
        }
    }

}

void EGLRender::SetFragmentShaderType(int paramType, int fShaderType) {
    LOGD("EGLRender::SetFragmentShaderType paramType = %d, param = %d", paramType, fShaderType)
    if (paramType == PARAM_TYPE_SHADER_INDEX) {
        if (fShaderType >= 0) {
            m_ShaderIndex = fShaderType % EGL_FEATURE_NUM;

            if (m_ProgramObj) {
                glDeleteProgram(m_ProgramObj);
                m_ProgramObj = GL_NONE;
            }

            m_ProgramObj = GLUtils::createProgram(&vertexShader, &m_fShaderStrs[m_ShaderIndex]);
            if (!m_ProgramObj) {
                GLUtils::checkGlError("Create Program");
                LOGD("EGLRender::SetFragmentShaderType Could not create program.")
                return;
            }

            m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
            GO_CHECK_GL_ERROR()
            m_TexSizeLoc = glGetUniformLocation(m_ProgramObj, "u_texSize");
            GO_CHECK_GL_ERROR()
        }
    }
}

void EGLRender::Draw() {
    LOGD("EGLRender::Draw")
    if (m_ProgramObj == GL_NONE) return;
    glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);

    glUseProgram(m_ProgramObj);

    // Do FBO off screen rendering
    // 绑定 FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);

    // 绑定 VAO 和图像纹理，进行绘制（离屏渲染）
    glBindVertexArray(m_VaoIds[0]);

    glActiveTexture(GL_TEXTURE0);
    // 绑定图像纹理
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_SamplerLoc, 0);

    if (m_TexSizeLoc > -1) {
        GLfloat size[2];
        size[0] = m_RenderImage.width;
        size[1] = m_RenderImage.height;
        glUniform2f(m_TexSizeLoc, size[0], size[1]);
    }


    //7. 渲染
    GO_CHECK_GL_ERROR()
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //一旦解绑 FBO 后面就不能调用 readPixels
    //glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

}

void EGLRender::UnInit() {
    LOGD("EGLRender::UnInit")
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = GL_NONE;
    }

    if (m_ImageTextureId) {
        glDeleteTextures(1, &m_ImageTextureId);
        m_ImageTextureId = GL_NONE;
    }

    if (m_FboTextureId) {
        glDeleteTextures(1, &m_FboTextureId);
        m_FboTextureId = GL_NONE;
    }

    if (m_VboIds[0]) {
        glDeleteBuffers(3, m_VboIds);
        m_VboIds[0] = GL_NONE;
        m_VboIds[1] = GL_NONE;
        m_VboIds[2] = GL_NONE;

    }

    if (m_VaoIds[0]) {
        glDeleteVertexArrays(1, m_VaoIds);
        m_VaoIds[0] = GL_NONE;
    }

    if (m_FboId) {
        glDeleteFramebuffers(1, &m_FboId);
        m_FboId = GL_NONE;
    }


    if (m_IsGLContextReady) {
        DestroyGlesEnv();
        m_IsGLContextReady = false;
    }

    //释放内存
    for(auto & m_fShaderStr : m_fShaderStrs){
        delete[] m_fShaderStr;
        m_fShaderStr = nullptr;
    }
}

void EGLRender::DestroyGlesEnv() {
    //8. 释放 EGL 环境
    if (m_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_eglDisplay, m_eglCtx);
        eglDestroySurface(m_eglDisplay, m_eglSurface);
        eglReleaseThread();
        eglTerminate(m_eglDisplay);
    }

    m_eglDisplay = EGL_NO_DISPLAY;
    m_eglSurface = EGL_NO_SURFACE;
    m_eglCtx = EGL_NO_CONTEXT;

}


