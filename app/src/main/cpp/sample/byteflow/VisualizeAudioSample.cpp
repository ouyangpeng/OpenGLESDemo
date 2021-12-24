//
// Created by OuyangPeng on 2021/12/20.
// 参考下面的博客：
// OpenGL ES 实现可视化实时音频 https://blog.csdn.net/Kennethdroid/article/details/106128767
//

#include "VisualizeAudioSample.h"
#include <thread>

VisualizeAudioSample::VisualizeAudioSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_pCurAudioData = nullptr;
    m_AudioDataSize = 0;

    m_pTextureCoords = nullptr;
    m_pVerticesCoords = nullptr;

    memset(m_VboIds, 0, sizeof(GLuint) * 2);

    m_FrameIndex = 0;
    m_pAudioBuffer = nullptr;
    m_bAudioDataReady = false;
}

VisualizeAudioSample::~VisualizeAudioSample() {
    if (m_pAudioBuffer != nullptr) {
        delete[] m_pAudioBuffer;
        m_pAudioBuffer = nullptr;
    }

    if (m_pTextureCoords != nullptr) {
        delete[] m_pTextureCoords;
        m_pTextureCoords = nullptr;
    }

    if (m_pVerticesCoords != nullptr) {
        delete[] m_pVerticesCoords;
        m_pVerticesCoords = nullptr;
    }
}

void VisualizeAudioSample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_visualize_audio.glsl");
    // 片段着色器脚本
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_visualize_audio.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGE("BigEyesSample::Init Create program fail")
        return;
    }
    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void VisualizeAudioSample::Draw() {
    LOGD("VisualizeAudioSample::Draw()")

    if (m_ProgramObj == GL_NONE) return;

    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    //加互斥锁，保证音频数据绘制与更新同步
    std::unique_lock<std::mutex> lock(m_Mutex);
    if (!m_bAudioDataReady) return;
    //根据音频数据更新纹理坐标和顶点坐标
    UpdateMesh();
    lock.unlock();

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) m_Width / (float) m_Height);

    // Generate VBO Ids and load the VBOs with data
    if (m_VboIds[0] == 0) {
        glGenBuffers(2, m_VboIds);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_RenderDataSize * 6 * 3, m_pVerticesCoords,
                     GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_RenderDataSize * 6 * 2, m_pTextureCoords,
                     GL_DYNAMIC_DRAW);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * m_RenderDataSize * 6 * 3,
                        m_pVerticesCoords);

        glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * m_RenderDataSize * 6 * 2,
                        m_pTextureCoords);
    }

    if (m_VaoId == GL_NONE) {
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


    // Use the program object
    glUseProgram(m_ProgramObj);
    glBindVertexArray(m_VaoId);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    GLUtils::setFloat(m_ProgramObj, "drawType", 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, m_RenderDataSize * 6);
    GLUtils::setFloat(m_ProgramObj, "drawType", 0.0f);
    glDrawArrays(GL_LINES, 0, m_RenderDataSize * 6);
}

void VisualizeAudioSample::Shutdown() {
    std::unique_lock<std::mutex> lock(m_Mutex);
    LOGD("m_Mutex addr = %d", &m_Mutex)
    m_Cond.notify_all();
    lock.unlock();

    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(2, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
    }
}

void VisualizeAudioSample::LoadAudioData(short *buffer, int len) {
    //GLSampleBase::LoadShortArrData(buffer, len);
    LOGD("VisualizeAudioSample::LoadShortArrData buffer=%p, len=%d", buffer, len)
    if (buffer == nullptr || len == 0)
        return;
    m_FrameIndex++;

    std::unique_lock<std::mutex> lock(m_Mutex);
    //前两帧数据直接填充 Buffer
    if (m_FrameIndex == 1) {
        m_pAudioBuffer = new short[len * 2];
        memcpy(m_pAudioBuffer, buffer, sizeof(short) * len);
        m_AudioDataSize = len;
        return;
    }
    //前两帧数据直接填充 Buffer
    if (m_FrameIndex == 2) {
        memcpy(m_pAudioBuffer + len, buffer, sizeof(short) * len);
        m_RenderDataSize = m_AudioDataSize / RESAMPLE_LEVEL;
        m_pVerticesCoords = new vec3[m_RenderDataSize * 6]; //(x,y,z) * 6 points
        m_pTextureCoords = new vec2[m_RenderDataSize * 6]; //(x,y) * 6 points
    }
    //将 Buffer2 中的数据拷贝的 Buffer1 中，并接收新的数据放到 Buffer2 中，
    if (m_FrameIndex > 2) {
        memcpy(m_pAudioBuffer, m_pAudioBuffer + len, sizeof(short) * len);
        memcpy(m_pAudioBuffer + len, buffer, sizeof(short) * len);
    }
    LOGD("VisualizeAudioSample::Draw() m_bAudioDataReady = true")
    //这个时候阻塞音频采集线程，通知渲染线程数据更新完毕
    m_bAudioDataReady = true;
    m_pCurAudioData = m_pAudioBuffer;
    m_Cond.wait(lock);
}

void VisualizeAudioSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                                 float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void
VisualizeAudioSample::UpdateMVPMatrix(mat4 &mvpMatrix, int angleX, int angleY, float ratio) const {
    LOGD("VisualizeAudioSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
         angleY, ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    mat4 Projection = ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //mat4 Projection = frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //mat4 Projection = perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    mat4 View = lookAt(
            vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            vec3(0, 0, 0), // and looks at the origin
            vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    mat4 Model = mat4(1.0f);
    Model = scale(Model, vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = rotate(Model, radiansX, vec3(1.0f, 0.0f, 0.0f));
    Model = rotate(Model, radiansY, vec3(0.0f, 1.0f, 0.0f));

//    auto radiansZ = static_cast<float>(MATH_PI / 2.0f);
    //Model = rotate(Model, radiansZ, vec3(0.0f, 0.0f, 1.0f));
    Model = translate(Model, vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = Projection * View * Model;
}

void VisualizeAudioSample::UpdateMesh() {
    //设置一个偏移步长
    int step = m_AudioDataSize / 64;
    //判断指针是否偏移到边界
    if (m_pAudioBuffer + m_AudioDataSize - m_pCurAudioData >= step) {
        LOGD("VisualizeAudioSample::Draw() m_pAudioBuffer + m_AudioDataSize - m_pCurAudioData >= step")
        float dy = 0.5f / MAX_AUDIO_LEVEL;
        float dx = 1.0f / m_RenderDataSize;
        for (int i = 0; i < m_RenderDataSize; ++i) {
            //RESAMPLE_LEVEL 表示采样间隔
            int index = i * RESAMPLE_LEVEL;
            float y = m_pCurAudioData[index] * dy * -1;
            //表示音频的数值转为正数
            y = y < 0 ? y : -y;

            //构建条状矩形的 4 个点
            vec2 p1(i * dx, 0 + 1.0f);
            vec2 p2(i * dx, y + 1.0f);
            vec2 p3((i + 1) * dx, y + 1.0f);
            vec2 p4((i + 1) * dx, 0 + 1.0f);

            //构建纹理坐标
            m_pTextureCoords[i * 6 + 0] = p1;
            m_pTextureCoords[i * 6 + 1] = p2;
            m_pTextureCoords[i * 6 + 2] = p4;
            m_pTextureCoords[i * 6 + 3] = p4;
            m_pTextureCoords[i * 6 + 4] = p2;
            m_pTextureCoords[i * 6 + 5] = p3;

            //构建顶点坐标，将纹理坐标转为顶点坐标
            m_pVerticesCoords[i * 6 + 0] = GLUtils::texCoordToVertexCoord(p1);
            m_pVerticesCoords[i * 6 + 1] = GLUtils::texCoordToVertexCoord(p2);
            m_pVerticesCoords[i * 6 + 2] = GLUtils::texCoordToVertexCoord(p4);
            m_pVerticesCoords[i * 6 + 3] = GLUtils::texCoordToVertexCoord(p4);
            m_pVerticesCoords[i * 6 + 4] = GLUtils::texCoordToVertexCoord(p2);
            m_pVerticesCoords[i * 6 + 5] = GLUtils::texCoordToVertexCoord(p3);
        }
        m_pCurAudioData += step;
    } else {
        //偏移到边界时，通知音频采集线程更新数据
        LOGD("VisualizeAudioSample::Draw() m_pAudioBuffer + m_AudioDataSize - m_pCurAudioData < step")
        m_bAudioDataReady = false;
        m_Cond.notify_all();
        return;
    }
}
