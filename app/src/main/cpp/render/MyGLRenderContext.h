//
// Created by OuyangPeng on 2021/11/26.
//

#ifndef OPENGLESDEMO_MYGLRENDERCONTEXT_H
#define OPENGLESDEMO_MYGLRENDERCONTEXT_H

#include <GLBaseSample.h>

class MyGLRenderContext
{
    MyGLRenderContext();

    ~MyGLRenderContext();

public:
    void SetRenderType(int sampleCategoryType, int renderSampleType);

    void OnSurfaceCreated(JNIEnv *env, jobject assetManager);

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

    static MyGLRenderContext* GetInstance();

    static void DestroyInstance();

    void SwitchBlendingMode();

    void SetDelta(float x, float y);

    void SetMinFilter(int filter);

    void SetMagFilter(int filter);

    void SetImageData(int format, int width, int height, uint8_t *pData);

    void SetImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData);

    void UpdateTransformMatrix(jfloat d, jfloat d1, jfloat d2, jfloat d3);

    void SetAudioData(short *buffer, int len);

    void SetTouchLocation(jfloat x, jfloat y);

private:
    static MyGLRenderContext *m_pContext;
    GLBaseSample *m_pBeforeSample;
    GLBaseSample *m_pCurSample;

    static NativeImage getImage(int format, int width, int height, uint8_t *pData) ;
};

#endif //OPENGLESDEMO_MYGLRENDERCONTEXT_H
