//
// Created by OuyangPeng on 2021/11/26.
//
#include <jni.h>
#include <MyGLRenderContext.h>
#include <EGLRender.h>

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSurfaceCreate(
        JNIEnv *env, jobject thiz, jobject asset_manager) {
    MyGLRenderContext::GetInstance()->OnSurfaceCreated(env, asset_manager);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSurfaceChange(
        JNIEnv *env, jobject thiz, jint width, jint height) {
    MyGLRenderContext::GetInstance()->OnSurfaceChanged(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeDrawFrame(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::GetInstance()->OnDrawFrame();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSetRenderType(
        JNIEnv *env, jobject thiz, jint sampleCategoryType, jint renderSampleType) {
    MyGLRenderContext::GetInstance()->SetRenderType(sampleCategoryType, renderSampleType);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeOnDestroy(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::DestroyInstance();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSwitchBlendingMode(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::GetInstance()->SwitchBlendingMode();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSetDelta(
        JNIEnv *env, jobject thiz, jfloat x, jfloat y) {
    MyGLRenderContext::GetInstance()->SetDelta(x, y);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSetMinFilter(
        JNIEnv *env, jobject thiz, jint filter) {
    MyGLRenderContext::GetInstance()->SetMinFilter(filter);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSetMagFilter(
        JNIEnv *env, jobject thiz, jint filter) {
    MyGLRenderContext::GetInstance()->SetMagFilter(filter);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSetImageData(
        JNIEnv *env, jobject thiz,
        jint format, jint width, jint height, jbyteArray imageData) {

    int len = env->GetArrayLength(imageData);
    u_int8_t *buf = new u_int8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));

    MyGLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
    delete[]buf;
    env->DeleteLocalRef(imageData);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_MyNativeRenderer_nativeSetImageDataWithIndex(
        JNIEnv *env, jobject thiz,
        jint index, jint format, jint width, jint height, jbyteArray imageData) {

    int len = env->GetArrayLength(imageData);
    u_int8_t *buf = new u_int8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));

    MyGLRenderContext::GetInstance()->SetImageDataWithIndex(index, format, width, height, buf);
    delete[]buf;
    env->DeleteLocalRef(imageData);
}


////////////////////////////////////////////   EGL 渲染相关 ////////////////////////////////////////////////////////////////////

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_egl_NativeEglRender_nativeEglRenderInit(JNIEnv *env, jobject thiz, jobject asset_manager) {
    EGLRender::GetInstance()->Init(env,asset_manager);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_egl_NativeEglRender_nativeEglRenderSetImageData(
        JNIEnv *env, jobject thiz, jbyteArray data, jint width, jint height) {

    int len = env->GetArrayLength (data);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));

    EGLRender::GetInstance()->SetImageData(buf, width, height);

    delete[] buf;
    env->DeleteLocalRef(data);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_egl_NativeEglRender_nativeEglRenderSetFragmentShaderType(
        JNIEnv *env, jobject thiz, jint param_type, jint fShaderType) {
    EGLRender::GetInstance()->SetFragmentShaderType(param_type, fShaderType);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_egl_NativeEglRender_nativeEglRenderDraw(JNIEnv *env, jobject thiz) {
    EGLRender::GetInstance()->Draw();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_render_egl_NativeEglRender_nativeEglRenderUnInit(JNIEnv *env, jobject thiz) {
    EGLRender::GetInstance()->UnInit();
}

////////////////////////////////////////////   EGL 渲染相关 ////////////////////////////////////////////////////////////////////