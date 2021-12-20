//
// Created by OuyangPeng on 2021/12/20.
// 参考博客：
// OpenGL ES 实现刮刮卡和手写板功能  https://blog.csdn.net/Kennethdroid/article/details/106339286

// 其实利用 Android Canvas 实现类似刮刮卡或者手写板功能比较方便，通过自定义 View 绘制 2 个图层，
// 位于上层的图层在手指划过的位置将透明度置为 0 ，这样下层图层的颜色便可以显示出来。
// 不过话又说回来，Android Canvas 实现类似刮刮卡功能虽然方便，但是性能一言难尽，
// 通常在复杂的应用界面不宜采用此类方法，此时就不得不考虑使用 OpenGL 进行优化。
//
// 本案例尝试使用 OpenGL 来实现类似刮刮卡的功能，
// 利用 OpenGL 根据手指滑动的坐标去构建一条一条的带状网格，然后基于此网格实现纹理映射。
// 为了使带状图形（网格）看起来平滑自然，我们还需要在起点和终点位置构建 2 个半圆，使滑动轨迹看起来平滑自然。
//

#include <GL_MyLock.h>
#include "ScratchCardSample.h"

/**
 * 求二元一次方程的系数
 * y1 = k * x1 + b => k = (y1 - b) / x1
 * y2 = k * x2 + b => y2 = ((y1 - b) / x1) * x2 + b
 */
vec2 binaryEquationGetKB(float x1, float y1, float x2, float y2) {
    float k = (y1 - y2) / (x1 - x2);
    float b = (x1 * y2 - x2 * y1) / (x1 - x2);
    return vec2(k, b);
}

/**
 * 一元二次方程求根
 * ax² + bx + c = 0
 */
vec2 quadEquationGetX(float a, float b, float c) {
    vec2 xArr;
    float result = pow(b, 2.0f) - 4 * a * c;
    if (result > 0) {
        xArr.x = (-b + sqrt(result)) / (2 * a);
        xArr.y = (-b - sqrt(result)) / (2 * a);
    } else if (result == 0) {
        xArr.y = xArr.x = (-b / (2 * a));
    }
    return xArr;
}

/**
 * 求圆和直线之间的交点
 * 直线方程：y = kx + b
 * 圆的方程：(x - m)² + (x - n)² = r²
 * x1, y1 = 线坐标1, x2, y2 = 线坐标2, m, n = 圆坐标, r = 半径
 */
vec4 getInsertPointBetweenCircleAndLine(float x1, float y1, float x2, float y2, float m, float n,
                                        float r) {
    vec2 kbArr = binaryEquationGetKB(x1, y1, x2, y2);
    float k = kbArr.x;
    float b = kbArr.y;

    float aX = 1 + k * k;
    float bX = 2 * k * (b - n) - 2 * m;
    float cX = m * m + (b - n) * (b - n) - r * r;

    vec4 insertPoints;
    vec2 xArr = quadEquationGetX(aX, bX, cX);
    insertPoints.x = xArr.x;
    insertPoints.y = k * xArr.x + b;
    insertPoints.z = xArr.y;
    insertPoints.w = k * xArr.y + b;

    return insertPoints;
}

ScratchCardSample::ScratchCardSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_bReset = true;
}

ScratchCardSample::~ScratchCardSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void ScratchCardSample::create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_coord_system.glsl");
    // 片段着色器脚本
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");

    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!mProgram) {
        LOGE("RotaryHeadSample::Init create program fail")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(mProgram, "s_TextureMap");
    m_MVPMatLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");

    //create RGBA texture
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
    glClearColor(0.2f, 0.2f, 0.2f, 1.0);
}

void ScratchCardSample::draw() {
    LOGD("ScratchCardSample::Draw()")
    if (mProgram == GL_NONE || m_TextureId == GL_NONE) return;

    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);

    if (m_VboIds[0] == GL_NONE) {
        // Generate VBO Ids and load the VBOs with data
        glGenBuffers(2, m_VboIds);
        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_pVtxCoords), m_pVtxCoords, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_pTexCoords), m_pTexCoords, GL_DYNAMIC_DRAW);
    }

    if (m_VaoId == GL_NONE) {
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

        glBindVertexArray(GL_NONE);
    }

    if (m_PointVector.size() < 1) return;

    // Use the program object
    glUseProgram(mProgram);
    // 绘制部分的逻辑，其中为了防止重复绘制，我们开启模板测试，
    // 下面代码设置的意思是，我们之前已经绘制过的位置，后面就不再进行重复绘制了。
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//当片段的模板值不为 1 时，片段通过测试进行渲染
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//若模板测试和深度测试都通过了，将片段对应的模板值替换为1
    glStencilMask(0xFF);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    for (auto pre_cur_point : m_PointVector) {
        CalculateMesh(vec2(pre_cur_point.x, pre_cur_point.y),
                      vec2(pre_cur_point.z, pre_cur_point.w));
        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_pVtxCoords), m_pVtxCoords);
        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_pTexCoords), m_pTexCoords);
        glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_NUM * 3);
    }
    glDisable(GL_STENCIL_TEST);
}

void ScratchCardSample::LoadImage(NativeImage *pImage) {
    LOGD("ScratchCardSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void ScratchCardSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteBuffers(2, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
    }
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void ScratchCardSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const {
    LOGD("ScratchCardSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
         angleY, ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

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

void
ScratchCardSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

/**
 * 构建网格
 * @param pre 和 cur 滑动轨迹中相邻的 2 个点
 */
void ScratchCardSample::CalculateMesh(vec2 pre, vec2 cur) {
    vec2 imgSize(m_RenderImage.width, m_RenderImage.height);
    vec2 p0 = pre * imgSize, p1 = cur * imgSize;
    vec2 v0, v1, v2, v3;
    auto r = static_cast<float>(EFFECT_RADIUS * imgSize.x);
    float x0 = p0.x, y0 = p0.y;
    float x1 = p1.x, y1 = p1.y;
    if (p0.y == p1.y) //1. 平行于 x 轴的
    {
        v0 = vec2(p0.x, p0.y - r) / imgSize;
        v1 = vec2(p0.x, p0.y + r) / imgSize;
        v2 = vec2(p1.x, p1.y - r) / imgSize;
        v3 = vec2(p1.x, p1.y + r) / imgSize;

    } else if (p0.x == p1.x) { //2. 平行于 y 轴的
        v0 = vec2(p0.x - r, p0.y) / imgSize;
        v1 = vec2(p0.x + r, p0.y) / imgSize;
        v2 = vec2(p1.x - r, p1.y) / imgSize;
        v3 = vec2(p1.x + r, p1.y) / imgSize;

    } else { //3. 其他 case
        float A0 = (y1 - y0) * y0 + (x1 - x0) * x0;
        float A1 = (y0 - y1) * y1 + (x0 - x1) * x1;
        // y = a0 * x + c0,  y = a1 * x + c1
        float a0 = -(x1 - x0) / (y1 - y0);
        float c0 = A0 / (y1 - y0);

        float a1 = -(x0 - x1) / (y0 - y1);
        float c1 = A1 / (y0 - y1);

        float x0_i = 0;
        float y0_i = a0 * x0_i + c0;

        float x1_i = 0;
        float y1_i = a1 * x1_i + c1;

        //计算直线与圆的交点
        vec4 v0_v1 = getInsertPointBetweenCircleAndLine(x0, y0, x0_i, y0_i, x0, y0, r);

        v0 = vec2(v0_v1.x, v0_v1.y) / imgSize;
        v1 = vec2(v0_v1.z, v0_v1.w) / imgSize;

        vec4 v2_v3 = getInsertPointBetweenCircleAndLine(x1, y1, x1_i, y1_i, x1, y1, r);

        v2 = vec2(v2_v3.x, v2_v3.y) / imgSize;
        v3 = vec2(v2_v3.z, v2_v3.w) / imgSize;

    }

    // 矩形 3 个三角形
    // 你或许会感到疑惑，一个矩形为什么要绘制 3 个三角形？
    // 这是因为点 V0、V1 的相对位置（谁在左边、谁在右边）我们并不知道，
    // 为了确保能绘制完整的矩形，这里直接绘制了 3 个三角形，这个后面还有优化。
    // 绘制部分的逻辑，其中为了防止重复绘制，我们开启模板测试
    m_pTexCoords[0] = v0;
    m_pTexCoords[1] = v1;
    m_pTexCoords[2] = v2;
    m_pTexCoords[3] = v0;
    m_pTexCoords[4] = v2;
    m_pTexCoords[5] = v3;
    m_pTexCoords[6] = v1;
    m_pTexCoords[7] = v2;
    m_pTexCoords[8] = v3;

    int index = 9;
    float step = MATH_PI / 10;
    // 2 个圆，一共 40 个三角形，360 度角平分 20 份
    for (int i = 0; i < 20; ++i) {
        float x = r * cos(i * step);
        float y = r * sin(i * step);

        float x_ = r * cos((i + 1) * step);
        float y_ = r * sin((i + 1) * step);

        x += x0;
        y += y0;
        x_ += x0;
        y_ += y0;

        m_pTexCoords[index + 6 * i + 0] = vec2(x, y) / imgSize;
        m_pTexCoords[index + 6 * i + 1] = vec2(x_, y_) / imgSize;
        m_pTexCoords[index + 6 * i + 2] = vec2(x0, y0) / imgSize;

        x = r * cos(i * step);
        y = r * sin(i * step);

        x_ = r * cos((i + 1) * step);
        y_ = r * sin((i + 1) * step);

        x += x1;
        y += y1;
        x_ += x1;
        y_ += y1;

        m_pTexCoords[index + 6 * i + 3] = vec2(x, y) / imgSize;
        m_pTexCoords[index + 6 * i + 4] = vec2(x_, y_) / imgSize;
        m_pTexCoords[index + 6 * i + 5] = vec2(x1, y1) / imgSize;
    }

    for (int i = 0; i < TRIANGLE_NUM * 3; ++i) {
        m_pVtxCoords[i] = GLUtils::texCoordToVertexCoord(m_pTexCoords[i]);
    }

}

void ScratchCardSample::SetTouchLocation(float x, float y) {
    GLBaseSample::SetTouchLocation(x, y);
    if (mWidth * mHeight != 0) {
        if (x == -1) m_bReset = true;

        if (m_bReset) {
            if (x != -1) {
                m_CurTouchPoint = vec2(x / mWidth, y / mHeight);
                m_bReset = false;
            }
        } else {
            m_PreTouchPoint = m_CurTouchPoint;
            m_CurTouchPoint = vec2(x / mWidth, y / mHeight);
            if (m_CurTouchPoint == m_PreTouchPoint)
                return;
            m_PointVector.emplace_back(m_PreTouchPoint.x, m_PreTouchPoint.y, m_CurTouchPoint.x,
                                       m_CurTouchPoint.y);
        }
    }

}