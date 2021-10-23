#include <cstring>
#include "../log/LogUtils.h"
#include "NativeTriangleVertextBufferObject.h"

// 可以参考这篇讲解： https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
namespace TRIANGLE_VERTEXT_BUFFER_OBJECT {
	void printGLString(const char* name, GLenum s) {
		const char* v = (const char*)glGetString(s);
		LOGI("GL %s = %s \n", name, v);
	}

	// 顶点着色器
	const char* vShaderStr =
		"#version 300 es                            \n"
		"layout(location = 0) in vec4 a_position;   \n"
		"layout(location = 1) in vec4 a_color;      \n"
		"uniform float u_offset;                    \n"
		"out vec4 v_color;                          \n"
		"void main()                                \n"
		"{                                          \n"
		"    v_color = a_color;                     \n"
		"    gl_Position = a_position;              \n"
		"    gl_Position.x += u_offset;             \n"
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
		"}";

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

		offsetLoc = glGetUniformLocation(mProgram, "u_offset");

		if (!mProgram) {
			LOGD("Could not create program");
			return;
		}

		vboIds[0] = 0;
		vboIds[1] = 0;

		// 设置清除颜色
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	void NativeTriangle::draw() {
		// 3 vertices, with (x,y,z) ,(r, g, b, a)  per-vertex
		GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
		{
			// 逆时针 三个顶点
            -0.5f, 0.5f, 0.0f,       // v0
            1.0f, 0.0f, 0.0f, 1.0f,  // c0

            -1.0f, -0.5f, 0.0f,        // v1
            0.0f, 1.0f, 0.0f, 1.0f,  // c1

            0.0f, -0.5f, 0.0f,        // v2
            0.0f, 0.0f, 1.0f, 1.0f,  // c2
		};

		// Index buffer data
		GLushort indices[3] = { 0, 1, 2 };

		// Set the viewport
		// 通知OpenGL ES 用于绘制的2D渲染表面的原点、宽度和高度。
		// 在OpenGL ES 中，视口(Viewport) 定义所有OpenGL ES 渲染操作最终显示的2D矩形
		// 视口(Viewport) 由原点坐标(x,y)和宽度(width) 、高度(height)定义。
		glViewport(0, 0, mWidth, mHeight);

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(mProgram);

		// without VBOs
		glUniform1f(offsetLoc, 0.0f);
		DrawPrimitiveWithoutVBOs(vertices,
			sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
			3, indices);

		// with VBOs
		// Offset tge vertex positions so both can be seen
		glUniform1f(offsetLoc, 1.0f);
		DrawPrimitiveWithVBOs(3, vertices,
			sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
			3, indices);
	}

	void NativeTriangle::change(int width, int height) {
		mWidth = width;
		mHeight = height;
		LOGD("Welcome to changew() width = %d , height = %d\n", width, height);
	}

	// vertices   - pointer to a buffer that contains vertex attribute data
	// vtxStride  - stride of attribute data / vertex in bytes
	// numIndices - number of indices that make up primitive drawn as triangles
	// indices    - pointer to element index buffer.
	void NativeTriangle::DrawPrimitiveWithoutVBOs(GLfloat* vertices,
		GLint vtxStride, GLint numIndices, GLushort* indices)
	{
		GLfloat* vtxBuf = vertices;

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(VERTEX_POS_INDX);
		glEnableVertexAttribArray(VERTEX_COLOR_INDX);

		glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
			GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

		vtxBuf += VERTEX_POS_SIZE;

		glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
			GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indices);

		glDisableVertexAttribArray(VERTEX_POS_INDX);
		glDisableVertexAttribArray(VERTEX_COLOR_INDX);
	}

	void NativeTriangle::DrawPrimitiveWithVBOs(GLint numVertices, GLfloat* vtxBuf,
	        GLint vtxStride, GLint numIndices, GLushort* indices)
	{
		GLuint offset = 0;

		// vboIds[0] - used to store vertex attribute data
		// vboIds[l] - used to store element indices
		if (vboIds[0] == 0 && vboIds[1] == 0)
		{
			//Only allocate on the first draw
			glGenBuffers(2, vboIds);

			glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
			glBufferData(GL_ARRAY_BUFFER, vtxStride * numVertices,
				vtxBuf, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				sizeof(GLushort) * numIndices,
				indices, GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

		glEnableVertexAttribArray(VERTEX_POS_INDX);
		glEnableVertexAttribArray(VERTEX_COLOR_INDX);

		glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
			GL_FLOAT, GL_FALSE, vtxStride, (const void*)offset);

		offset += VERTEX_POS_SIZE * sizeof(GLfloat);
		glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
			GL_FLOAT, GL_FALSE, vtxStride, (const void*)offset);

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(VERTEX_POS_INDX);
		glDisableVertexAttribArray(VERTEX_COLOR_INDX);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

// ====================================================================

static TRIANGLE_VERTEXT_BUFFER_OBJECT::NativeTriangle* nativeTriangle;


extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleVertexBufferObjectNativeRenderer_00024Companion_nativeSurfaceCreate(
	JNIEnv * env, jobject thiz) {
	nativeTriangle = new TRIANGLE_VERTEXT_BUFFER_OBJECT::NativeTriangle();
	if (nativeTriangle != nullptr) {
		nativeTriangle->create();
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleVertexBufferObjectNativeRenderer_00024Companion_nativeSurfaceChange(
	JNIEnv * env, jobject thiz, jint width, jint height) {
	if (nativeTriangle != nullptr) {
		nativeTriangle->change(width, height);
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_triangle_HelloTriangleVertexBufferObjectNativeRenderer_00024Companion_nativeDrawFrame(
	JNIEnv * env, jobject thiz) {
	if (nativeTriangle != nullptr) {
		nativeTriangle->draw();
	}
}