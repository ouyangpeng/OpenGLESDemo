//
// Created by OuyangPeng on 2021/12/10.
//

#include <glm/ext.hpp>
#include "RotateTexture.h"

#define TEXTURE_MAP_VERTEX_POS_INDEX              0
#define TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX        1

#define TEXTURE_MAP_VERTEX_POS_SIZE               3
#define TEXTURE_MAP_VERTEX_TEXTCOORD_SIZE         2

//顶点坐标
static GLfloat verticesCoords[] = {
        -1.0f, -1.0f, 0.0f, // bottom left
        1.0f, -1.0f, 0.0f, // bottom right
        -1.0f, 1.0f, 0.0f, // top left
        1.0f, 1.0f, 0.0f, // top right
};

//正常纹理坐标
static GLfloat textureCoords[] = {
        0.0f, 1.0f, // bottom left
        1.0f, 1.0f, // bottom right
        0.0f, 0.0f, // top left
        1.0f, 0.0f, // top right
};

// 由于 OpenGLES 绘制是以三角形为单位的，设置绘制的 2 个三角形为 V0V1V2 和 V0V2V3 。
// 当我们调整纹理坐标的顺序顶点坐标顺序不变，如 T0T1T2T3 -> T1T2T3T0 ，绘制后将得到一个顺时针旋转 90 度的纹理贴图。
// 所以调整纹理坐标和顶点坐标的对应关系可以实现纹理图简单的旋转。

// 注意索引从0开始!
static GLushort indices[] = {
        0, 1, 2,        // 第一个三角形
        1, 3, 2         // 第二个三角形
};

RotateTexture::RotateTexture() {

    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;

    mLastTime = 0;
    angle = 0;
}

RotateTexture::~RotateTexture() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}


void RotateTexture::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_rotate_texture.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (m_ProgramObj == GL_NONE) {
        LOGE("RotateTexture::Init m_ProgramObj == GL_NONE")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");

    // ======================= 设置纹理===================================
    // Create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void RotateTexture::Draw() {
    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 每次绘制之前先update一下, 计算最新的旋转角度
    update(getDeltaTime());

    // 更新顶点坐标
    updateVertexCoordinates();

    // Use the program object
    glUseProgram(m_ProgramObj);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    glVertexAttribPointer(TEXTURE_MAP_VERTEX_POS_INDEX, TEXTURE_MAP_VERTEX_POS_SIZE,
                          GL_FLOAT, GL_FALSE,
                          TEXTURE_MAP_VERTEX_POS_SIZE * sizeof(GLfloat), verticesCoords);

    // Load the texture coordinate
    glVertexAttribPointer(TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX, TEXTURE_MAP_VERTEX_TEXTCOORD_SIZE,
                          GL_FLOAT, GL_FALSE,
                          TEXTURE_MAP_VERTEX_TEXTCOORD_SIZE * sizeof(GLfloat), textureCoords);

    glEnableVertexAttribArray(TEXTURE_MAP_VERTEX_POS_INDEX);
    glEnableVertexAttribArray(TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

    glDisableVertexAttribArray(TEXTURE_MAP_VERTEX_POS_INDEX);
    glDisableVertexAttribArray(TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX);

    glBindVertexArray(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void RotateTexture::updateVertexCoordinates() {// 实际旋转角度
    auto rotateRadianAngle = translateToRadianAngle((int) angle);
    LOGD("RotateTexture 旋转角度为：%f",rotateRadianAngle)

//    https://www.cnblogs.com/nanyangzp/p/4071518.html
//
//    以(x0,y0)为旋转中心点，
//    已经知旋转前点的位置(x1,y1)和旋转的角度a，求旋转后点的新位置(x2,y2)
//
//    如果是逆时针旋转：
//    x2 = (x1 - x0) * cosa - (y1 - y0) * sina + x0
//    y2 = (y1 - y0) * cosa + (x1 - x0) * sina + y0
//
//    如果是顺时针旋转：
//    x2 = (x1 - x0) * cosa + (y1 - y0) * sina + x0
//    y2 = (y1 - y0) * cosa - (x1 - x0) * sina + y0


    // 更新顶点坐标  我们是逆时针旋转，所以按照下面的公式计算旋转后的4个顶点位置
    //  x2 = (x1 - x0) * cosa - (y1 - y0) * sina + x0
    //  y2 = (y1 - y0) * cosa + (x1 - x0) * sina + y0
    float halfWidth = (float) m_Width / 2;
    float halfHeight = (float) m_Height / 2;

    float leftBottomX =
            (-halfWidth) * cos(rotateRadianAngle) - (-halfHeight) * sin(rotateRadianAngle);
    float leftBottomY =
            (-halfHeight) * cos(rotateRadianAngle) + (-halfWidth) * sin(rotateRadianAngle);

    float rightBottomX =
            (halfWidth) * cos(rotateRadianAngle) - (-halfHeight) * sin(rotateRadianAngle);
    float rightBottomY =
            (-halfHeight) * cos(rotateRadianAngle) + (halfWidth) * sin(rotateRadianAngle);

    float leftTopX =
            (-halfWidth) * cos(rotateRadianAngle) - (halfHeight) * sin(rotateRadianAngle);
    float leftTopY =
            (halfHeight) * cos(rotateRadianAngle) + (-halfWidth) * sin(rotateRadianAngle);

    float rightTopX =
            (halfWidth) * cos(rotateRadianAngle) - (halfHeight) * sin(rotateRadianAngle);
    float rightTopY =
            (halfHeight) * cos(rotateRadianAngle) + (halfWidth) * sin(rotateRadianAngle);

    // 还得转归一化坐标
    // bottom left
    verticesCoords[0] = leftBottomX / halfWidth;
    verticesCoords[1] = leftBottomY / halfHeight;
    verticesCoords[2] = 0;
    // bottom right
    verticesCoords[3] = rightBottomX / halfWidth;
    verticesCoords[4] = rightBottomY / halfHeight;
    verticesCoords[5] = 0;
    // top left
    verticesCoords[6] = leftTopX / halfWidth;
    verticesCoords[7] = leftTopY / halfHeight;
    verticesCoords[8] = 0;
    // top right
    verticesCoords[9] = rightTopX / halfWidth;
    verticesCoords[10] = rightTopY / halfHeight;
    verticesCoords[11] = 0;

   LOGD("RotateTexture 顶点属性 为：v1(%f,%f),v2(%f,%f),v3(%f,%f),v4(%f,%f),",
         verticesCoords[0], verticesCoords[1], verticesCoords[3], verticesCoords[4],
         verticesCoords[6], verticesCoords[7], verticesCoords[9], verticesCoords[10])
}


void RotateTexture::Shutdown() {
    GLBaseSample::Shutdown();
    glDeleteTextures(1, &m_TextureId);
}

void RotateTexture::LoadImage(NativeImage *pImage) {
    LOGD("RotateTexture::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

float RotateTexture::getDeltaTime() {
    if (mLastTime == 0) {
        mLastTime = TimeUtils::currentTimeMillis();
    }
    long currentTime = TimeUtils::currentTimeMillis();
    long elapsedTime = currentTime - mLastTime;
    float deltaTime = (float) elapsedTime / 1000.0f;
    mLastTime = currentTime;
    return deltaTime;
}

void RotateTexture::update(float deltaTime) {
    LOGD("RotateTexture  update() deltaTime：%f",deltaTime)
    // Compute a rotation angle based on time to rotate the cube
    // 根据旋转的时间计算旋转角度
    angle += (deltaTime * 40.0f);
}

float RotateTexture::translateToRadianAngle(int rotateAngle) {
    // 最多360度
    rotateAngle = rotateAngle % 360;
    // 转化为弧度角
    auto rotateRadianAngle = static_cast<float>(MATH_PI / 180.0f * rotateAngle);
    return rotateRadianAngle;
}
