//
// Created by OuyangPeng on 2021/11/26.
//
#include <SimpleTexture2D.h>
#include <NativeTriangle.h>
#include <NativeTriangle2.h>
#include <NativeTriangle3.h>
#include <NativeTriangleMapBuffers.h>
#include <NativeTriangleVertextArrayObject.h>
#include <NativeTriangleVertextBufferObject.h>
#include <NativeCubeSimpleVertexShader.h>
#include <SimpleTextureCubeMap.h>
#include <MipMap2D.h>
#include <TextureWrap.h>
#include <MultiTexture.h>
#include <ParticleSystem.h>
#include <ParticleSystemTransformFeedBack.h>

#include "MyGLRenderContext.h"
#include "../utils/GLUtils.h"


MyGLRenderContext *MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext() {
    LOGD("MyGLRenderContext::MyGLRenderContext")
    m_pCurSample = new NativeTriangle();
    m_pBeforeSample = nullptr;

}

MyGLRenderContext::~MyGLRenderContext() {
    LOGD("MyGLRenderContext::~MyGLRenderContext")
    if (m_pCurSample) {
        delete m_pCurSample;
        m_pCurSample = nullptr;
    }

    if (m_pBeforeSample) {
        delete m_pBeforeSample;
        m_pBeforeSample = nullptr;
    }

}


void MyGLRenderContext::SetParamsInt(int paramType, int value0, int value1) {
    LOGD("MyGLRenderContext::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType,
         value0, value1)

    if (paramType == SAMPLE_TYPE) {
        m_pBeforeSample = m_pCurSample;

        LOGD("MyGLRenderContext::SetParamsInt 0 m_pBeforeSample = %p", m_pBeforeSample)

        switch (value0) {
            case SAMPLE_TYPE_KEY_TRIANGLE:
                m_pCurSample = new NativeTriangle();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE2:
                m_pCurSample = new NativeTriangle2();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE3:
                m_pCurSample = new NativeTriangle3();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE_MAP_BUFFERS:
                m_pCurSample = new NativeTriangleMapBuffers();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE_VERTEX_ARRAY_OBJECT:
                m_pCurSample = new NativeTriangleVAO();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE_VERTEX_BUFFER_OBJECT:
                m_pCurSample = new NativeTriangleVBO();
                break;
            case SAMPLE_TYPE_KEY_CUBE_SIMPLE_VERTEX_SHADER:
                m_pCurSample = new NativeCubeSimpleVertexShader();
                break;
            case SAMPLE_TYPE_KEY_SIMPLE_TEXTURE_2D:
                m_pCurSample = new SimpleTexture2D();
                break;
            case SAMPLE_TYPE_KEY_SIMPLE_TEXTURE_CUBE_MAP:
                m_pCurSample = new SimpleTextureCubeMap();
                break;
            case SAMPLE_TYPE_KEY_MIPMAP_2D:
                m_pCurSample = new MipMap2D();
                break;
            case SAMPLE_TYPE_KEY_TEXTURE_WRAP:
                m_pCurSample = new TextureWrap();
                break;
            case SAMPLE_TYPE_KEY_MULTI_TEXTURE:
                m_pCurSample = new MultiTexture();
                break;
            case SAMPLE_TYPE_KEY_PARTICLE_SYSTEM:
                m_pCurSample = new ParticleSystem();
                break;
            case SAMPLE_TYPE_KEY_PARTICLE_SYSTEM_TRANSFORM_FEEDBACK:
                m_pCurSample = new ParticleSystemTransformFeedBack();
                break;
            default:
                m_pCurSample = nullptr;
                break;
        }

        LOGD("MyGLRenderContext::SetParamsInt m_pBeforeSample = %p, m_pCurSample=%p",
             m_pBeforeSample, m_pCurSample)
    }
}


void MyGLRenderContext::OnSurfaceCreated(JNIEnv *env, jobject assetManager) {
    LOGD("MyGLRenderContext::OnSurfaceCreated")

    // 初始化设置assetManager  一定要记得初始化，否则会报空指针异常
    GLUtils::setEnvAndAssetManager(env, assetManager);

    if (m_pBeforeSample) {
        m_pBeforeSample->shutdown();
        delete m_pBeforeSample;
        m_pBeforeSample = nullptr;
    }

    if (m_pCurSample) {
        m_pCurSample->create();
    }
}

void MyGLRenderContext::OnSurfaceChanged(int width, int height) {
    LOGD("MyGLRenderContext::OnSurfaceChanged [w, h] = [%d, %d]", width, height)
    if (m_pCurSample) {
        m_pCurSample->change(width, height);
    }
}

void MyGLRenderContext::OnDrawFrame() {
//    LOGD("MyGLRenderContext::OnDrawFrame")
    if (m_pCurSample) {
        m_pCurSample->draw();
    }
}

MyGLRenderContext *MyGLRenderContext::GetInstance() {
//    LOGD("MyGLRenderContext::GetInstance")
    if (m_pContext == nullptr) {
        m_pContext = new MyGLRenderContext();
    }
    return m_pContext;
}

void MyGLRenderContext::DestroyInstance() {
    LOGD("MyGLRenderContext::DestroyInstance")
    if (m_pContext) {
        delete m_pContext;
        m_pContext = nullptr;
    }

}