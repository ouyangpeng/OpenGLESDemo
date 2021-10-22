#include <cstring>
#include "../log/LogUtils.h"
#include "NativeTriangleMapBuffers.h"
#include "NativeTriangleVertextArrayObject.h"


// 可以参考这篇讲解： https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
namespace TRIANGLE_VERTEXT_ARRAY_OBJECT {
	void printGLString(const char* name, GLenum s) {
		const char* v = (const char*)glGetString(s);
		LOGI("GL %s = %s \n", name, v);
	}

	// 顶点着色器
	const char* vShaderStr =
			"#version 300 es                            \n"
			"layout(location = 0) in vec4 a_position;   \n"
			"layout(location = 1) in vec4 a_color;      \n"
			"out vec4 v_color;                          \n"
			"void main()                                \n"
			"{                                          \n"
			"    v_color = a_color;                     \n"
			"    gl_Position = a_position;              \n"
			"}";

	// 片段着色器
	const char* fShaderStr =
			"#version 300 es            \n"
			"precision mediump float;   \n"
			"in vec4 v_color;           \n"
			"out vec4 o_fragColor;      \n"
			"void main()                \n"
			"{                          \n"
			"    o_fragColor = v_color; \n"
			"}" ;

    // 3 vertices, with (x,y,z) ,(r, g, b, a)  per-vertex
    GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
            {
                    // 逆时针 三个顶点
                    0.0f, 0.5f, 0.0f,            // v0 上角
                    1.0f, 0.0f, 0.0f, 1.0f,      // c0

                    -0.5f, -0.5f, 0.0f,          // v1 左下角
                    0.0f, 1.0f, 0.0f, 1.0f,      // c1

                    0.5f, -0.5f, 0.0f,           // v2 右下角
                    0.0f, 0.0f, 1.0f, 1.0f       // c2
            };

    // Index buffer data
    GLushort indices[3] = { 0, 1, 2 };


    NativeTriangle::NativeTriangle() {

    }

    NativeTriangle::~NativeTriangle() {

    }

	void NativeTriangle::create() {
		printGLString("Version", GL_VERSION);
		printGLString("Vendor", GL_VENDOR);
		printGLString("Renderer", GL_RENDERER);
		printGLString("Extensions", GL_EXTENSIONS);

		mProgram = GLUtils::createProgram(&vShaderStr, &fShaderStr);
		if (!mProgram) {
			LOGD("Could not create program");
			return;
		}

        //  Generate VBO Ids and load the VBOs with data
        glGenBuffers(2,vboIds);

		glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIds[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

        // Generate VAO Id
        glGenVertexArrays(1, &vaoId);

        // Bind the VAO and then setup the vertex attributes
        glBindVertexArray(vaoId);

        glBindBuffer(GL_ARRAY_BUFFER,vboIds[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIds[1]);

        glEnableVertexAttribArray(VERTEX_POS_INDX);
        glEnableVertexAttribArray(VERTEX_COLOR_INDX);

        glVertexAttribPointer(VERTEX_POS_INDX,VERTEX_POS_SIZE,
                GL_FLOAT,GL_FALSE,VERTEX_STRIDE,(const void *) 0);

        glVertexAttribPointer(VERTEX_COLOR_INDX,VERTEX_COLOR_SIZE,
                              GL_FLOAT,GL_FALSE,VERTEX_STRIDE,(const void *) (VERTEX_POS_SIZE * sizeof(GLfloat)));

        // Reset to the default VAO
        glBindVertexArray(0);

		// 设置清除颜色
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	void NativeTriangle::draw() {
		// Set the viewport
		// 通知OpenGL ES 用于绘制的2D渲染表面的原点、宽度和高度。
		// 在OpenGL ES 中，视口(Viewport) 定义所有OpenGL ES 渲染操作最终显示的2D矩形
		// 视口(Viewport) 由原点坐标(x,y)和宽度(width) 、高度(height)定义。
		glViewport(0, 0, mWidth, mHeight);

		// Clear the color buffer
		// 清除屏幕
		// 在OpenGL ES中，绘图中涉及多种缓冲区类型：颜色、深度、模板。
		// 这个例子，绘制三角形，只向颜色缓冲区中绘制图形。在每个帧的开始，我们用glClear函数清除颜色缓冲区
		// 缓冲区将用glClearColor指定的颜色清除。
		// 这个例子，我们调用了GLES30.glClearColor(1.0f, 1.0f, 1.0f, 0.0f); 因此屏幕清为白色。
		// 清除颜色应该由应用程序在调用颜色缓冲区的glClear之前设置。
		glClear(GL_COLOR_BUFFER_BIT);

		// Use the program object
		// 在glUseProgram函数调用之后，每个着色器调用和渲染调用都会使用这个程序对象（也就是之前写的着色器)了。
		// 当我们渲染一个物体时要使用着色器程序 , 将其设置为活动程序。这样就可以开始渲染了
		glUseProgram(mProgram);

		// Bind the VAO
		glBindVertexArray(vaoId);

		// Draw with the VAO settins
		glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_SHORT,(const void *) 0);

		// Return to the default VAO
		glBindVertexArray(0);
	}

    void NativeTriangle::change(int width, int height) {
        mWidth = width;
        mHeight = height;
        LOGD("Welcome to changew() width = %d , height = %d\n", width, height);
    }
}

// ====================================================================

static TRIANGLE_VERTEXT_ARRAY_OBJECT::NativeTriangle* nativeTriangle;

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleVertexArrayObjectNativeRenderer_00024Companion_nativeSurfaceCreate(
	JNIEnv * env, jobject thiz) {
	nativeTriangle = new  TRIANGLE_VERTEXT_ARRAY_OBJECT::NativeTriangle();
	if (nativeTriangle != nullptr) {
		nativeTriangle->create();
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleVertexArrayObjectNativeRenderer_00024Companion_nativeSurfaceChange(
	JNIEnv * env, jobject thiz, jint width, jint height) {
	if (nativeTriangle != nullptr) {
		nativeTriangle->change(width, height);
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleVertexArrayObjectNativeRenderer_00024Companion_nativeDrawFrame(
	JNIEnv * env, jobject thiz) {
	if (nativeTriangle != nullptr) {
		nativeTriangle->draw();
	}
}