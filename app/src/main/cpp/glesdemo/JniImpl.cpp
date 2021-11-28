//
// Created by OuyangPeng on 2021/11/26.
//
#include <jni.h>
#include "MyGLRenderContext.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeSurfaceCreate(
        JNIEnv *env, jobject thiz, jobject asset_manager) {
    MyGLRenderContext::GetInstance()->OnSurfaceCreated(env, asset_manager);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeSurfaceChange(
        JNIEnv *env, jobject thiz, jint width, jint height) {
    MyGLRenderContext::GetInstance()->OnSurfaceChanged(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeDrawFrame(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::GetInstance()->OnDrawFrame();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeSetParamsInt(
        JNIEnv *env, jobject thiz, jint paramType, jint value0, jint value1) {
    MyGLRenderContext::GetInstance()->SetParamsInt(paramType, value0, value1);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeOnDestroy(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::DestroyInstance();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeSwitchBlendingMode(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::GetInstance()->SwitchBlendingMode();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeSetDelta(
        JNIEnv *env, jobject thiz, jfloat x, jfloat y) {
    MyGLRenderContext::GetInstance()->SetDelta(x, y);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeSetMinFilter(
        JNIEnv *env, jobject thiz, jint filter) {
    MyGLRenderContext::GetInstance()->SetMinFilter(filter);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_MyNativeRenderer_nativeSetMagFilter(
        JNIEnv *env, jobject thiz, jint filter) {
    MyGLRenderContext::GetInstance()->SetMagFilter(filter);
}