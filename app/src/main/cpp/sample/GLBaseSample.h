//
// Created by OuyangPeng on 2021/11/26.
//

#ifndef OPENGLESDEMO_GLBASESAMPLE_H
#define OPENGLESDEMO_GLBASESAMPLE_H

#include <GLUtils.h>
#include <ImageDef.h>
#include "GLBaseSampleType.h"

// 注意，这个目录在java层创建，参考 com.oyp.openglesdemo.activity.NativeRenderActivity.onResume方法
#define DEFAULT_OGL_ASSETS_DIR "/data/data/com.oyp.openglesdemo/cache"

class GLBaseSample {
public:
    GLBaseSample() {
        m_ProgramObj = 0;
        m_Width = 0;
        m_Height = 0;
    }

    virtual ~GLBaseSample() {

    }

    virtual void Create() = 0;

    virtual void Change(int width, int height) {
        LOGD("Change() width = %d , height = %d\n", width, height)
        m_Width = width;
        m_Height = height;
        // Set the viewport
        // 通知OpenGL ES 用于绘制的2D渲染表面的原点、宽度和高度。
        // 在OpenGL ES 中，视口(Viewport) 定义所有OpenGL ES 渲染操作最终显示的2D矩形
        // 视口(Viewport) 由原点坐标(x,y)和宽度(width) 、高度(height)定义。
        glViewport(0, 0, m_Width, m_Height);
    }

    virtual void Draw() = 0;

    virtual void Shutdown() = 0;

    // 默认啥都不做，等待有需要的子类去重写
    virtual void SwitchBlendingMode() {}

    virtual void SetDelta(float x, float y) {}

    virtual void SetMinFilter(int filter) {}

    virtual void SetMagFilter(int filter) {}

    virtual void LoadImage(NativeImage *pImage) {};

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage) {}

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {}

    virtual void LoadAudioData(short *buffer, int len) {}

    virtual void SetTouchLocation(float x, float y) {}

    virtual void SetGravityXY(float x, float y) {}

protected:
    /**
     * 程序对象
     */
    GLuint m_ProgramObj;

    /**
     * 顶点着色器
     */
    const char *VERTEX_SHADER;
    /**
     * 片段着色器脚本
     */
    const char *FRAGMENT_SHADER;

    /**
     * 屏幕宽度
     */
    int m_Width;
    /**
     * 屏幕高度
     */
    int m_Height;
};


#endif //OPENGLESDEMO_GLBASESAMPLE_H
