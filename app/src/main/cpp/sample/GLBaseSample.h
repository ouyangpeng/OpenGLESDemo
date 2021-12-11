//
// Created by OuyangPeng on 2021/11/26.
//

#ifndef OPENGLESDEMO_GLBASESAMPLE_H
#define OPENGLESDEMO_GLBASESAMPLE_H

#include <GLUtils.h>
#include <ImageDef.h>
#include "GLBaseSampleType.h"

class GLBaseSample {
public:
    GLBaseSample() {
        mProgram = 0;
        mWidth = 0;
        mHeight = 0;
    }

    virtual ~GLBaseSample() {

    }

    virtual void create() = 0;

    virtual void change(int width, int height) {
        LOGD("change() width = %d , height = %d\n", width, height)
        mWidth = width;
        mHeight = height;
        // Set the viewport
        // 通知OpenGL ES 用于绘制的2D渲染表面的原点、宽度和高度。
        // 在OpenGL ES 中，视口(Viewport) 定义所有OpenGL ES 渲染操作最终显示的2D矩形
        // 视口(Viewport) 由原点坐标(x,y)和宽度(width) 、高度(height)定义。
        glViewport(0, 0, mWidth, mHeight);
    }

    virtual void draw() = 0;

    virtual void shutdown() = 0;

    // 默认啥都不做，等待有需要的子类去重写
    virtual void switchBlendingMode() {}

    virtual void setDelta(float x, float y) {}

    virtual void setMinFilter(int filter) {}

    virtual void setMagFilter(int filter) {}

    virtual void LoadImage(NativeImage *pImage) {};

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage) {}

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {}

protected:
    GLuint mProgram;
    int mWidth;
    int mHeight;

    const char *VERTEX_SHADER;
    const char *FRAGMENT_SHADER;
};


#endif //OPENGLESDEMO_GLBASESAMPLE_H
