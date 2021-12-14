//
// Created by OuyangPeng on 2021/12/14.
// 参考链接：
// NDK OpenGL ES 3.0 开发（二十二）：PBO   https://blog.csdn.net/Kennethdroid/article/details/103931627
// https://blog.csdn.net/u012278016/article/details/106129709
// PBO是OpenGL最高效的像素拷贝方式吗？ https://www.jianshu.com/p/3be97e897531
// OpenGL深入探索——像素缓冲区对象 （PBO） https://blog.csdn.net/panda1234lee/article/details/51546502

/**
 *
PBO 是什么：
    OpenGL PBO（Pixel Buffer Object），被称为像素缓冲区对象，主要被用于异步像素传输操作。
    PBO 仅用于执行像素传输，不连接到纹理，且与 FBO （帧缓冲区对象）无关。
    OpenGL PBO（像素缓冲区对象）类似于 VBO（顶点缓冲区对象），PBO 开辟的也是 GPU 缓存，而存储的是图像数据。

    与 PBO 绑定相关的 Target 标签有 2 个：GL_PIXEL_UNPACK_BUFFER 和 GL_PIXEL_PACK_BUFFER 。
    其中将 PBO 绑定为 GL_PIXEL_UNPACK_BUFFER 时，glTexImage2D() 和 glTexSubImage2D() 表示从 PBO 中解包（unpack）像素数据并复制到帧缓冲区 。
    将 PBO 绑定为 GL_PIXEL_PACK_BUFFER 时，glReadPixels() 表示从帧缓冲区中读取像素数据并打包进（pack） PBO 。

为什么要用 PBO：
    在 OpenGL 开发中，特别是在低端平台上处理高分辨率的图像时，图像数据在内存和显存之前拷贝往往会造成性能瓶颈，而利用 PBO 可以在一定程度上解决这个问题。
    使用 PBO 可以在 GPU 的缓存间快速传递像素数据，不影响 CPU 时钟周期，除此之外，PBO 还支持异步传输。

1. opengl2.0之后才有的，PBO设计的目的就是快速地向显卡传输数据，或者从显卡读取数据，我们可以使用它更加高效的读取屏幕数据。
2. 单个PBO读取屏幕数据效率大概和 glReadPixels() 差不多，双PBO交换读取效率会很高。
        原因是使用PBO时，屏幕上的数据不是读取到内存，而是从显卡读到PBO中，或者如果内部机制是读取到内存中，
        但这也是由DMA 控制器来完成的，而不是cpu指令来做的，再加上两个PBO交换使用，所以读取效率很高。

        PBO的主要优点是可以通过DMA (Direct Memory Access)快速地在显卡上传递像素数据，而不影响CPU的时钟周期（中断）。
        另一个优势是它还具备异步DMA传输。
        也正因为这个特性，使得在使用单个PBO的情况下，性能提升并不明显，所以通常需要两个PBO配合使用。

3. 还可以使用PBO 高效读取内存数据(比如纹理)到显存中。

PBO 坑点：  https://www.jianshu.com/p/3be97e897531
然而PBO还有一个非常坑的地方，经测试表明，在部分硬件上glMapBufferRange映射出来的Buffer拷贝极为耗时，可以高达30+ms，
 这对于音视频处理显然是不能接受的。通常，映射出来的是一个DirectByteBuffer，也是一个堆外内存（C内存），
 这部分内存本身只能通过Buffer.get(byte[])拷贝来拿到数据，但正常情况下只需要2-3ms。出现这种问题估计是硬件上留下的坑。
所以，在Android上使用PBO是有比较多的兼容性问题的，包括上面说的。

正确使用PBO的方式是，首先判断是否支持PBO，如果支持，则还是先使用glReadPixels进行读取测试，
 记录平均耗时，然后再使用PBO进行读取测试，记录平均耗时，最后对比两个方式的耗时，选择最快的一个。
 这样动态处理是比较复杂的，然而在这种情况下你不得不这样做。那么有没有一种既简单又高效的方式呢？
 在Android平台，提供了更为高效的像素数据读取方法，也就是ImageReader。
 */

#include "PBOSample.h"

//#define PBO_UPLOAD
#define PBO_DOWNLOAD

#define VERTEX_POS_INDX  0
#define TEXTURE_POS_INDX 1

PBOSample::PBOSample() {
    m_ImageTextureId = GL_NONE;
    m_FboTextureId = GL_NONE;
    m_SamplerLoc = GL_NONE;
    m_FboId = GL_NONE;
    m_FboProgramObj = GL_NONE;
    m_FboSamplerLoc = GL_NONE;
    m_MVPMatrixLoc = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_FrameIndex = 0;
}

PBOSample::~PBOSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void PBOSample::LoadImage(NativeImage *pImage) {
    LOGD("PBOSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void PBOSample::create() {
    //顶点坐标
    GLfloat vVertices[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
    };

    //正常纹理坐标
    GLfloat vTexCoors[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    };

    //fbo 纹理坐标与正常纹理方向不同，原点位于左下角
    GLfloat vFboTexCoors[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
    };

    GLushort indices[] = {0, 1, 2, 1, 3, 2};

    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_pbo.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    // 编译链接用于普通渲染的着色器程序
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    // 用于离屏渲染的顶点着色器脚本，不使用变换矩阵
    const char *vFboShaderStr = GLUtils::openTextFile(
            "vertex/vertex_shader_texture_map.glsl");
    // 用于离屏渲染的片段着色器脚本，取每个像素的灰度值
    const char *fFboFragmentShader = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_fbo.glsl");
    // 编译链接用于离屏渲染的着色器程序
    m_FboProgramObj = GLUtils::createProgram(&vFboShaderStr, &fFboFragmentShader);

    if (mProgram == GL_NONE || m_FboProgramObj == GL_NONE) {
        LOGD("PBOSample::Init mProgram == GL_NONE")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(mProgram, "s_TextureMap");
    m_MVPMatrixLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");
    m_FboSamplerLoc = glGetUniformLocation(m_FboProgramObj, "s_TextureMap");

    //=================================== VBO 相关 ===================================

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

    //=================================== VAO 相关 ===================================

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

    // 创建并初始化图像纹理
    glGenTextures(1, &m_ImageTextureId);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    GO_CHECK_GL_ERROR()


    //=================================== PBO 相关 ===================================
    // 加载图像数据到纹理对象时，CPU 负责将图像数据拷贝到 PBO ，而 GPU 负责将图像数据从 PBO 传送到纹理对象。
    // 所以，当我们使用多个 PBO 时，通过交换 PBO 的方式进行拷贝和传送，可以实现这两步操作同时进行。

    int imgByteSize = m_RenderImage.width * m_RenderImage.height * 4;//RGBA
    //初始化 FBO
    glGenBuffers(2, m_UploadPboIds);
    // 将 PBO 绑定为 GL_PIXEL_UNPACK_BUFFER 时，
    // glTexImage2D() 和 glTexSubImage2D() 表示从 PBO 中解包（unpack）像素数据并复制到帧缓冲区。
    // 即： 绑定为 GL_PIXEL_UNPACK_BUFFER 表示该 PBO 用于将像素数据从程序传送到 OpenGL 中

    //绑定
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_UploadPboIds[0]);
    // 分配显存空间
    glBufferData(GL_PIXEL_UNPACK_BUFFER, imgByteSize, nullptr, GL_STREAM_DRAW);
    //绑定
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_UploadPboIds[1]);
    // 分配显存空间
    glBufferData(GL_PIXEL_UNPACK_BUFFER, imgByteSize, nullptr, GL_STREAM_DRAW);
    // 解绑
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glGenBuffers(2, m_DownloadPboIds);
    // 将 PBO 绑定为 GL_PIXEL_PACK_BUFFER 时，
    // glReadPixels() 表示从帧缓冲区中读取像素数据并打包进（pack） PBO。
    // 即： 绑定为 GL_PIXEL_PACK_BUFFER 表示该 PBO 用于从 OpenGL 中读回像素数据。
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_DownloadPboIds[0]);
    // 分配显存空间
    glBufferData(GL_PIXEL_PACK_BUFFER, imgByteSize, nullptr, GL_STREAM_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_DownloadPboIds[1]);
    // 分配显存空间
    glBufferData(GL_PIXEL_PACK_BUFFER, imgByteSize, nullptr, GL_STREAM_READ);
    // 解绑
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    if (!CreateFrameBufferObj()) {
        LOGD("PBOSample::Init CreateFrameBufferObj fail")
        return;
    }
}

void PBOSample::draw() {
    // 离屏渲染
    //glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glViewport(0, 0, m_RenderImage.width, m_RenderImage.height);

    //Upload
    UploadPixels();
    GO_CHECK_GL_ERROR()

    // Do FBO off screen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
    glUseProgram(m_FboProgramObj);
    glBindVertexArray(m_VaoIds[1]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
    glUniform1i(m_FboSamplerLoc, 0);
    GO_CHECK_GL_ERROR()
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Download
    DownloadPixels();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 普通渲染
    // Do normal rendering
    glViewport(0, 0, mWidth, mHeight);
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);
    glUseProgram(mProgram);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(m_VaoIds[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
    glUniformMatrix4fv(m_MVPMatrixLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1i(m_SamplerLoc, 0);
    GO_CHECK_GL_ERROR()
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    GO_CHECK_GL_ERROR()
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindVertexArray(GL_NONE);

    m_FrameIndex++;
}

void PBOSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        mProgram = GL_NONE;
    }

    if (m_FboProgramObj) {
        glDeleteProgram(m_FboProgramObj);
        m_FboProgramObj = GL_NONE;
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

    if (m_DownloadPboIds[0]) {
        glDeleteBuffers(2, m_DownloadPboIds);
    }

    if (m_UploadPboIds[0]) {
        glDeleteBuffers(2, m_UploadPboIds);
    }
}

bool PBOSample::CreateFrameBufferObj() {
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGD("PBOSample::CreateFrameBufferObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE")
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    return true;
}

void PBOSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const {
    LOGD("PBOSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, 100);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 1), // Camera is at (0,0,1), in World Space
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

void PBOSample::UploadPixels() {
    LOGD("PBOSample::UploadPixels")

#ifdef PBO_UPLOAD
    //使用 `glTexSubImage2D` 将图像数据从 PBO1 传送到纹理对象
    int dataSize = m_RenderImage.width * m_RenderImage.height * 4;
    //交换索引
    // "index" 用于从PBO中拷贝像素数据至texture object
    // "nextIndex" 用于更新另一个PBO中的像素数据
    int index = m_FrameIndex % 2;
    int nextIndex = (index + 1) % 2;
    BEGIN_TIME("PBOSample::UploadPixels Copy Pixels from PBO to Textrure Obj")
        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
        // 绑定PBO
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_UploadPboIds[index]);
        // 调用 glTexSubImage2D 后立即返回，不影响 CPU 时钟周期
        // 为什么要用glTexSubImage2D呢，因为如果用glTexImage2D，glTexImage2D会销毁纹理内存重新申请，
        // glTexSubImage2D就仅仅只是更新纹理中的数据，这就提高了速度，并且优化了显存的利用率
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        m_RenderImage.width, m_RenderImage.height,
                        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    END_TIME("PBOSample::UploadPixels Copy Pixels from PBO to Textrure Obj")
#else
    BEGIN_TIME("PBOSample::UploadPixels Copy Pixels from System Mem to Textrure Obj")
        glBindTexture(GL_TEXTURE_2D, m_ImageTextureId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_RenderImage.width, m_RenderImage.height, GL_RGBA,
                        GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    END_TIME("PBOSample::UploadPixels Copy Pixels from System Mem to Textrure Obj")
#endif

#ifdef PBO_UPLOAD
    //更新图像数据，复制到 PBO 中
    BEGIN_TIME("PBOSample::UploadPixels Update Image data")
    // 指定pbo
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_UploadPboIds[nextIndex]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, dataSize, nullptr, GL_STREAM_DRAW);
    // 做一个map映射把PBO的数据和内存的data指针进行关联
    auto *bufPtr = (GLubyte *) glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0,
                                                   dataSize,
                                                   GL_MAP_WRITE_BIT |
                                                   GL_MAP_INVALIDATE_BUFFER_BIT);
    GO_CHECK_GL_ERROR()
    LOGD("PBOSample::UploadPixels bufPtr=%p",bufPtr)
    if(bufPtr)
    {
        GO_CHECK_GL_ERROR();
        memcpy(bufPtr, m_RenderImage.ppPlane[0], static_cast<size_t>(dataSize));

        //update image data
        int randomRow = random() % (m_RenderImage.height - 5);
        memset(bufPtr + randomRow * m_RenderImage.width * 4, 188,
                    static_cast<size_t>(m_RenderImage.width * 4 * 5));
        //取消映射
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    END_TIME("PBOSample::UploadPixels Update Image data")

#else
    NativeImage nativeImage = m_RenderImage;
    NativeImageUtil::AllocNativeImage(&nativeImage);
    BEGIN_TIME("PBOSample::UploadPixels Update Image data")
        //update image data
        int randomRow = random() % (m_RenderImage.height - 5);
        memset(m_RenderImage.ppPlane[0] + randomRow * m_RenderImage.width * 4, 188,
               static_cast<size_t>(m_RenderImage.width * 4 * 5));
        NativeImageUtil::CopyNativeImage(&m_RenderImage, &nativeImage);
    END_TIME("PBOSample::UploadPixels Update Image data")
    NativeImageUtil::FreeNativeImage(&nativeImage);
#endif
}

void PBOSample::DownloadPixels() {
    int dataSize = m_RenderImage.width * m_RenderImage.height * 4;
    NativeImage nativeImage = m_RenderImage;
    nativeImage.format = IMAGE_FORMAT_RGBA;


    auto *pBuffer = new uint8_t[dataSize];
    nativeImage.ppPlane[0] = pBuffer;
    BEGIN_TIME("DownloadPixels glReadPixels without PBO")
        // lReadPixels是OpenGL ES 2.0和OpenGL ES 3.0都支持的api，使用最为简单广泛，
        // 只需要绑定一个FBO，然后就可以通过glReadPixels来读取像素数据到一个指定的缓冲区就可以了。
        // 这是本文所有方式中最为低效的，但因为其简单通用，所以使用广泛。
        glReadPixels(0, 0, nativeImage.width, nativeImage.height, GL_RGBA, GL_UNSIGNED_BYTE,
                     pBuffer);
        //NativeImageUtil::DumpNativeImage(&nativeImage, "/sdcard/DCIM", "Normal");
    END_TIME("DownloadPixels glReadPixels without PBO")
    delete[]pBuffer;

    //交换 PBO
    int index = m_FrameIndex % 2;
    int nextIndex = (index + 1) % 2;

    //将图像数据从帧缓冲区读回到 PBO 中
    BEGIN_TIME("DownloadPixels glReadPixels with PBO")
        // 先绑定第一个PBO
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_DownloadPboIds[index]);
        // 然后调用另一个特殊的glReadPixels异步读取像素数据，这时候会立即返回，而不是像第一种方法那样需要等待。
        glReadPixels(0, 0, m_RenderImage.width, m_RenderImage.height,
                        GL_RGBA, GL_UNSIGNED_BYTE,nullptr);
    END_TIME("DownloadPixels glReadPixels with PBO")

    BEGIN_TIME("DownloadPixels PBO glMapBufferRange")
        // 绑定到第二个PBO
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_DownloadPboIds[nextIndex]);
        // glMapBufferRange会等待DMA传输完成，所以需要交替使用pbo
        // 映射内存
        auto *bufPtr = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0,
                                        dataSize,GL_MAP_READ_BIT);

        if (bufPtr) {
            nativeImage.ppPlane[0] = static_cast<uint8_t *>(bufPtr);
            //NativeImageUtil::DumpNativeImage(&nativeImage, "/sdcard/DCIM", "PBO");
            // 解除映射
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        }
    END_TIME("DownloadPixels PBO glMapBufferRange")
    //解除绑定PBO
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}
