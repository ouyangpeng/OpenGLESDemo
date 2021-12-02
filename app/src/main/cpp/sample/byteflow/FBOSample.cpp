//
// Created by OuyangPeng on 2021/12/1.
//

/*
 * 参考链接：
 * NDK OpenGL ES 3.0 开发（五）：FBO 离屏渲染  https://blog.csdn.net/Kennethdroid/article/details/98883854
 * 帧缓冲 : https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/05%20Framebuffers/
 *
 *  ============什么是 FBO============
 *  FBO（Frame Buffer Object）即帧缓冲区对象，实际上是一个可添加缓冲区的容器，可以为其添加纹理或渲染缓冲区对象（RBO）。
    FBO 本身不能用于渲染，只有添加了纹理或者渲染缓冲区之后才能作为渲染目标，它仅且提供了 3 种附着（Attachment），分别是颜色附着、深度附着和模板附着。
    RBO（Render Buffer Object）即渲染缓冲区对象，是一个由应用程序分配的 2D 图像缓冲区。
    渲染缓冲区可以用于分配和存储颜色、深度或者模板值，可以用作 FBO 中的颜色、深度或者模板附着。
    使用 FBO 作为渲染目标时，首先需要为 FBO 的附着添加连接对象，如颜色附着需要连接纹理或者渲染缓冲区对象的颜色缓冲区。

    ============为什么用 FBO============
    默认情况下，OpenGL ES 通过绘制到窗口系统提供的帧缓冲区，
    然后将帧缓冲区的对应区域复制到纹理来实现渲染到纹理，但是此方法只有在纹理尺寸小于或等于帧缓冲区尺寸才有效。

    另一种方式是通过使用连接到纹理的 pbuffer 来实现渲染到纹理，但是与上下文和窗口系统提供的可绘制表面切换开销也很大。
    因此，引入了帧缓冲区对象 FBO 来解决这个问题。

    Android OpenGL ES 开发中，一般使用 GLSurfaceView 将绘制结果显示到屏幕上，
    然而在实际应用中，也有许多场景不需要渲染到屏幕上，
    如利用 GPU 在后台完成一些图像转换、缩放等耗时操作，这个时候利用 FBO 可以方便实现类似需求。
    使用 FBO 可以让渲染操作不用再渲染到屏幕上，而是渲染到离屏 Buffer 中，
    然后可以使用 glReadPixels 或者 HardwareBuffer 将渲染后的图像数据读出来，从而实现在后台利用 GPU 完成对图像的处理。
 */


#include "FBOSample.h"

FBOSample::FBOSample() {
    m_VaoIds[0] = GL_NONE;
    m_VboIds[0] = GL_NONE;

    m_ImageTextureId = GL_NONE;
    m_FboTextureId = GL_NONE;
    m_SamplerLoc = GL_NONE;
    m_FboId = GL_NONE;
    m_FboProgramObj = GL_NONE;
    m_FboSamplerLoc = GL_NONE;
}

FBOSample::~FBOSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void FBOSample::LoadImage(NativeImage *pImage) {
    LOGD("FBOSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage)
    {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void FBOSample::create() {
    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_texture_map.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    // 用于离屏渲染的片段着色器脚本，取每个像素的灰度值
    const char * m_FboFragmentShader =  GLUtils::openTextFile(
            "fragment/fragment_shader_texture_fbo.glsl");

    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    // 编译链接用于离屏渲染的着色器程序
    m_FboProgramObj =GLUtils::createProgram(&VERTEX_SHADER, &m_FboFragmentShader);

    if (mProgram == GL_NONE || m_FboProgramObj == GL_NONE)
    {
        LOGE("FBOSample::Init mProgram == GL_NONE")
        return;
    }
    m_SamplerLoc = glGetUniformLocation(mProgram, "s_TextureMap");
    m_FboSamplerLoc = glGetUniformLocation(m_FboProgramObj, "s_TextureMap");

    // 生成 VBO ，加载顶点数据和索引数据
    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(4, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vVertices), fbo_vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vTexCoors), fbo_vTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vFboTexCoors), fbo_vFboTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fbo_indices), fbo_indices, GL_STATIC_DRAW);

    GO_CHECK_GL_ERROR()

    // 生成 2 个 VAO，一个用于普通渲染，另一个用于离屏渲染
    // Generate VAO Ids
    glGenVertexArrays(2, m_VaoIds);
    // 初始化用于普通渲染的 VAO
    // Normal rendering VAO
    glBindVertexArray(m_VaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(FBO_VERTEX_POS_INDX);
    glVertexAttribPointer(FBO_VERTEX_POS_INDX, FBO_VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE,
                          FBO_VERTEX_POS_SIZE * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(FBO_TEXTURE_POS_INDX);
    glVertexAttribPointer(FBO_TEXTURE_POS_INDX, FBO_TEXTURE_POS_SIZE, GL_FLOAT, GL_FALSE,
                          FBO_TEXTURE_POS_SIZE * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(GL_NONE);


    // 初始化用于离屏渲染的 VAO
    // FBO off screen rendering VAO
    glBindVertexArray(m_VaoIds[1]);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(FBO_VERTEX_POS_INDX);
    glVertexAttribPointer(FBO_VERTEX_POS_INDX, FBO_VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE,
                          FBO_VERTEX_POS_SIZE * sizeof(GLfloat),  nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glEnableVertexAttribArray(FBO_TEXTURE_POS_INDX);
    glVertexAttribPointer(FBO_TEXTURE_POS_INDX, FBO_TEXTURE_POS_SIZE, GL_FLOAT, GL_FALSE,
                          FBO_TEXTURE_POS_SIZE * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[3]);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(GL_NONE);

    // 创建并初始化图像纹理
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR()

    if (!CreateFrameBufferObj())
    {
        LOGD("FBOSample::Init CreateFrameBufferObj fail")
        return;
    }

}

void FBOSample::draw() {
    FUN_BEGIN_TIME(" FBOSample::draw()")
        // ====================离屏渲染====================
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);

        // Do FBO off screen rendering
        // 绑定 FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);

        // 选定离屏渲染的 Program，绑定 VAO 和图像纹理，进行绘制（离屏渲染）
        // m_ImageTextureId 为另外一个用于纹理映射的图片纹理
        glUseProgram(m_FboProgramObj);
        glBindVertexArray(m_VaoIds[1]);
        glActiveTexture(GL_TEXTURE0);

        // 绑定图像纹理
        glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
        glUniform1i(m_FboSamplerLoc, 0);
        GO_CHECK_GL_ERROR()
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
        GO_CHECK_GL_ERROR()
        glBindVertexArray(GL_NONE);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        // 解绑 FBO
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

        // ====================普通渲染====================
        // Do normal rendering
        glViewport(0, 0, mWidth, mHeight);

        // 完成离屏渲染后，结果图数据便保存在我们之前连接到 FBO 的纹理 m_FboTextureId 。
        // 我们再拿 FBO 纹理 m_FboTextureId 做一次普通渲染便可将之前离屏渲染的结果绘制到屏幕上。
        // 这里我们编译连接了 2 个 program ,一个用作离屏渲染的 m_FboProgramObj，一个用于普通渲染的 mProgram

        glUseProgram(mProgram);
        GO_CHECK_GL_ERROR()
        glBindVertexArray(m_VaoIds[0]);
        glActiveTexture(GL_TEXTURE0);
        //绑定 FBO 纹理
        glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
        glUniform1i(m_SamplerLoc, 0);
        GO_CHECK_GL_ERROR()
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
        GO_CHECK_GL_ERROR()
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        glBindVertexArray(GL_NONE);
    FUN_END_TIME(" FBOSample::draw()")
}

void FBOSample::shutdown() {
    if (mProgram)
    {
        glDeleteProgram(mProgram);
    }

    if (m_FboProgramObj)
    {
        glDeleteProgram(m_FboProgramObj);
    }

    if (m_ImageTextureId)
    {
        glDeleteTextures(1, &m_ImageTextureId);
    }

    if (m_FboTextureId)
    {
        glDeleteTextures(1, &m_FboTextureId);
    }

    if (m_VboIds[0])
    {
        glDeleteBuffers(4, m_VboIds);
    }

    if (m_VaoIds[0])
    {
        glDeleteVertexArrays(2, m_VaoIds);
    }

    if (m_FboId)
    {
        glDeleteFramebuffers(1, &m_FboId);
    }

    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

// 创建并初始化 FBO
bool FBOSample::CreateFrameBufferObj()
{
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);
    // 分配内存大小
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 m_RenderImage.width, m_RenderImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // 检查帧缓冲FBO是否是完整的，如果不是，我们将打印错误信息。
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE) {
        LOGD("FBOSample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
        return false;
    }
    // 解绑纹理
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    // 解绑 FBO   记得要解绑帧缓冲，保证我们不会不小心渲染到错误的帧缓冲上。
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return true;
}


