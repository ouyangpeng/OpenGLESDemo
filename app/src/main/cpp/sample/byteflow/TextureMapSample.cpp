//
// Created by OuyangPeng on 2021/11/30.
// 参考： https://blog.csdn.net/Kennethdroid/article/details/96887637
//

#include "TextureMapSample.h"

TextureMapSample::TextureMapSample() {
    mTextureId = GL_NONE;
}

TextureMapSample::~TextureMapSample() {
    NativeImageUtil::FreeNativeImage(&mRenderImage);
}

void TextureMapSample::create() {
    GLUtils::printGLInfo();

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_texture_map.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }

    // Get the uniform locations
    mSamplerLoc = glGetUniformLocation(mProgram, "s_TextureMap");

    /*
     * 纹理映射的一般步骤：
            1、生成纹理，编译链接着色器程序
            2、确定纹理坐标及对应的顶点坐标
            3、加载图像数据到纹理，加载纹理坐标和顶点坐标到着色器程序
            4、绘制
     */

    // 生成一个纹理，将纹理 id 赋值给 m_TextureId
    glGenTextures(1, &mTextureId);
    // 将纹理 m_TextureId 绑定到类型 GL_TEXTURE_2D 纹理
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    //设置纹理 S 轴（横轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //设置纹理 T 轴（纵轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //设置纹理采样方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void TextureMapSample::draw() {
    FUN_BEGIN_TIME("TextureMapSample::draw()")
        LOGD("TextureMapSample::draw()")
        if (mProgram == GL_NONE || mTextureId == GL_NONE) return;

        // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
        glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the program object
        glUseProgram(mProgram);

        // upload RGBA image data
        // 采样器统一变量将加载一个指定纹理绑定的纹理单元的数值：
        // 例如，用数字0指定采样器表示从单元GL_TEXTURE0读取，指定数字1指定采样器表示从单元GL_TEXTURE1读取，以此类推
        // 纹理用glActiveTexture函数绑定到纹理单元
        glActiveTexture(GL_TEXTURE0);
        // 将纹理绑定到当前活动单元
        glBindTexture(GL_TEXTURE_2D, mTextureId);

        //加载 RGBA 格式的图像数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderImage.width, mRenderImage.height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, mRenderImage.ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        // Load the vertex position
        glVertexAttribPointer(TEXTURE_MAP_VERTEX_POS_INDEX, TEXTURE_MAP_VERTEX_POS_SIZE,
                              GL_FLOAT, GL_FALSE,
                              TEXTURE_MAP_VERTEX_POS_SIZE * sizeof(GLfloat), verticesCoords);

        // Load the texture coordinate
        glVertexAttribPointer(TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX, TEXTURE_MAP_VERTEX_TEXTCOORD_SIZE,
                              GL_FLOAT, GL_FALSE,
                              TEXTURE_MAP_VERTEX_TEXTCOORD_SIZE * sizeof(GLfloat), textureCoords);

        glEnableVertexAttribArray(TEXTURE_MAP_VERTEX_POS_INDEX);
        glEnableVertexAttribArray(TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX);

        // Bind the RGBA map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTextureId);

        glUniform1i(mSamplerLoc, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    FUN_END_TIME("TextureMapSample::draw()")
}

void TextureMapSample::shutdown() {
    if (mTextureId) {
        // 删除纹理
        glDeleteTextures(1, &mTextureId);
        mTextureId = GL_NONE;
    }
    GLUtils::DeleteProgram(mProgram);
}

void TextureMapSample::LoadImage(NativeImage *pImage) {
    LOGD("TextureMapSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        mRenderImage.width = pImage->width;
        mRenderImage.height = pImage->height;
        mRenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &mRenderImage);
    }
}
