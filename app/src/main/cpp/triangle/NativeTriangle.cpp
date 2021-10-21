#include "NativeTriangle.h"
#include "../graphics/LogUtils.h"
#include "../graphics/GLUtils.h"

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s \n", name, v);
}

// 顶点着色器
const char *VERTEX_SHADER_TRIANGLE =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 vPosition;  \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vPosition;              \n"
        "}                                        \n";

// 片段着色器
const char *FRAGMENT_SHADER_TRIANGLE =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
        "}                                            \n";

GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                         -0.5f, -0.5f, 0.0f,
                         0.5f, -0.5f, 0.0f
};

NativeTriangle::NativeTriangle() {

}

NativeTriangle::~NativeTriangle() {

}

void NativeTriangle::create() {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    mProgram = GLUtils::createProgram(&VERTEX_SHADER_TRIANGLE,&FRAGMENT_SHADER_TRIANGLE);
    if (!mProgram){
        LOGD("Could not create program");
        return;
    }
}

void NativeTriangle::draw() {
    glClearColor(1.0f,1.0f,1.0f,0.0f);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(mProgram);

    // Load the vertex data
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,vVertices);

    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES,0,3);
}


// ====================================================================

static NativeTriangle * nativeTriangle;
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleNativeRenderer_00024Companion_nativeSurfaceCreate(
        JNIEnv *env, jobject thiz) {
    nativeTriangle= new NativeTriangle();
    if(nativeTriangle !=nullptr){
        nativeTriangle->create();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleNativeRenderer_00024Companion_nativeSurfaceChange(
        JNIEnv *env, jobject thiz, jint width, jint height) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleNativeRenderer_00024Companion_nativeDrawFrame(
        JNIEnv *env, jobject thiz) {
    if(nativeTriangle !=nullptr){
        nativeTriangle->draw();
    }
}




