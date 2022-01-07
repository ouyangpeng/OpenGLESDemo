//
// Created by OuyangPeng on 2021/12/1.
//

// 参考链接
// 理解YUV  https://blog.csdn.net/weixin_43752854/article/details/84841514
// 一文掌握 YUV 图像的基本处理  https://blog.csdn.net/Kennethdroid/article/details/94031821
// OpenGL ES 3.0 开发（三）：YUV 渲染  https://blog.csdn.net/Kennethdroid/article/details/97153407
#include <GLBaseSample.h>
#include "NV21TextureMapSample.h"

NV21TextureMapSample::NV21TextureMapSample() {
    m_yTextureId = GL_NONE;
    m_uvTextureId = GL_NONE;

    m_ySamplerLoc = GL_NONE;
    m_uvSamplerLoc = GL_NONE;
}

NV21TextureMapSample::~NV21TextureMapSample() {
    NativeImageUtil::FreeNativeImage(&mRenderImage);
}

void NV21TextureMapSample::LoadImage(NativeImage *pImage) {
    LOGD("TextureMapSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage)
    {
        mRenderImage.width = pImage->width;
        mRenderImage.height = pImage->height;
        mRenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &mRenderImage);
    }
}

void NV21TextureMapSample::Create() {
    GLUtils::printGLInfo();

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_texture_map.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_yuv_render.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }

    // Get the sampler location
    m_ySamplerLoc = glGetUniformLocation (m_ProgramObj, "y_texture" );
    m_uvSamplerLoc = glGetUniformLocation(m_ProgramObj, "uv_texture");

    // 生成2个纹理，将纹理 id 赋值给 textureIds
    GLuint textureIds[2] = {0};
    glGenTextures(2, textureIds);

    m_yTextureId = textureIds[0];
    m_uvTextureId = textureIds[1];

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NV21TextureMapSample::Draw() {
    LOGD("NV21TextureMapSample::Draw()")

    if(m_ProgramObj == GL_NONE || m_yTextureId == GL_NONE || m_uvTextureId == GL_NONE) return;

    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram(m_ProgramObj);

    //upload Y plane data
    // 将纹理 m_TextureId 绑定到类型 GL_TEXTURE_2D 纹理
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    //加载 RGBA 格式的图像数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, mRenderImage.width, mRenderImage.height,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, mRenderImage.ppPlane[0]);
    //设置纹理 S 轴（横轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //设置纹理 T 轴（纵轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //设置纹理采样方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //update UV plane data
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);
    //加载 RGBA 格式的图像数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, mRenderImage.width >> 1, mRenderImage.height >> 1,
                 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, mRenderImage.ppPlane[1]);
    //设置纹理 S 轴（横轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //设置纹理 T 轴（纵轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //设置纹理采样方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // Load the vertex position
    glVertexAttribPointer(NV21_TEXTURE_MAP_VERTEX_POS_INDEX, NV21_TEXTURE_MAP_VERTEX_POS_SIZE,
                          GL_FLOAT, GL_FALSE,
                          NV21_TEXTURE_MAP_VERTEX_POS_SIZE * sizeof(GLfloat), yuv_verticesCoords);

    // Load the texture coordinate
    glVertexAttribPointer(NV21_TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX, NV21_TEXTURE_MAP_VERTEX_TEXTCOORD_SIZE,
                          GL_FLOAT, GL_FALSE,
                          NV21_TEXTURE_MAP_VERTEX_TEXTCOORD_SIZE * sizeof(GLfloat), yuv_textureCoords);

    glEnableVertexAttribArray(NV21_TEXTURE_MAP_VERTEX_POS_INDEX);
    glEnableVertexAttribArray(NV21_TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX);

    // Bind the Y plane map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    // Set the Y plane sampler to texture unit to 0
    glUniform1i(m_ySamplerLoc, 0);

    // Bind the UV plane map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);
    // Set the UV plane sampler to texture unit to 1
    glUniform1i(m_uvSamplerLoc, 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, yuv_indices);
}

void NV21TextureMapSample::Shutdown() {
    GLBaseSample::Shutdown();

    if (m_yTextureId) {
        // 删除纹理
        glDeleteTextures(1, &m_yTextureId);
        m_yTextureId = GL_NONE;
    }

    if (m_uvTextureId) {
        // 删除纹理
        glDeleteTextures(1, &m_uvTextureId);
        m_uvTextureId = GL_NONE;
    }
}
