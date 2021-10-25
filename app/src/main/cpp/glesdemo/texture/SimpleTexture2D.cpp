//
// Created by OuyangPeng on 2021/10/24.
//

#include "SimpleTexture2D.h"

namespace NAMESPACE_SimpleTexture2D {
	// 顶点着色器
	const char* VERTEX_SHADER_TRIANGLE =
		"#version 300 es                            \n"
		"layout(location = 0) in vec4 a_position;   \n"
		"layout(location = 1) in vec2 a_texCoord;   \n"
		"out vec2 v_texCoord;                       \n"
		"void main()                                \n"
		"{                                          \n"
		"   gl_Position = a_position;               \n"
		"   v_texCoord = a_texCoord;                \n"
		"}                                          \n";

	// 片段着色器
	const char* FRAGMENT_SHADER_TRIANGLE =
		"#version 300 es                                     \n"
		"precision mediump float;                            \n"
		"in vec2 v_texCoord;                                 \n"
		"layout(location = 0) out vec4 outColor;             \n"
		"uniform sampler2D s_texture;                        \n"
		"void main()                                         \n"
		"{                                                   \n"
		"  outColor = texture( s_texture, v_texCoord );      \n"
		"}                                                   \n";

	SimpleTexture2D::SimpleTexture2D() {

	}

	SimpleTexture2D::~SimpleTexture2D() {

	}

	void SimpleTexture2D::create() {
		GLUtils::printGLString("Version", GL_VERSION);
		GLUtils::printGLString("Vendor", GL_VENDOR);
		GLUtils::printGLString("Renderer", GL_RENDERER);
		GLUtils::printGLString("Extensions", GL_EXTENSIONS);

		programObject = GLUtils::createProgram(&VERTEX_SHADER_TRIANGLE, &FRAGMENT_SHADER_TRIANGLE);
		if (!programObject) {
			LOGD("Could not create program");
			return;
		}
		// Get the sampler location
		samplerLoc = glGetUniformLocation(programObject, "s_texture");

		// Load the texture  加载纹理
		textureId = CreateSimpleTexture2D();

		// 设置清除颜色
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}



	void SimpleTexture2D::draw() {
		GLfloat vVertices[] =
		{
			-0.5f,  0.5f, 0.0f,     // Position 0
			0.0f,  0.0f,            // TexCoord 0

			-0.5f, -0.5f, 0.0f,     // Position 1
			0.0f,  1.0f,            // TexCoord 1

			0.5f, -0.5f, 0.0f,      // Position 2
			1.0f,  1.0f,            // TexCoord 2

			0.5f,  0.5f, 0.0f,      // Position 3
			1.0f,  0.0f             // TexCoord 3
		};
		GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

		// Set the viewport
		glViewport(0, 0, mWidth, mHeight);

		// Clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// Use the program object
		glUseProgram(programObject);

		// Load the vertext position
		glVertexAttribPointer(0, 3, GL_FLOAT,
			GL_FALSE, 5 * sizeof(GLfloat), vVertices);

		// Load the texture coordinate
		glVertexAttribPointer(1, 2, GL_FLOAT,
			GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Bind the texture
		// 激活当前纹理单元，以便后续的glBindTexture调用将纹理绑定到当前活动单元
		glActiveTexture(GL_TEXTURE0);
		// 纹理绑定到纹理单元0
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Set the sampler texture unit to 0
		// 采样器设置为纹理单元0
		glUniform1i(samplerLoc, 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
	}

	void SimpleTexture2D::change(int width, int height) {
		mWidth = width;
		mHeight = height;
		LOGD("change() width = %d , height = %d\n", width, height);
	}

	// Create a simple 2x2 texture image with four different colors
	GLuint SimpleTexture2D::CreateSimpleTexture2D()
	{
		//Texture object handle
		GLuint textureId;

		// 2x2 Image, 3 bytes per pixel (R,G,B)
		GLubyte pixels[4 * 3] =
		{
			  255,   0,   0, // Red
				0, 255,   0, // Green
				0,   0, 255, // Blue
			  255, 255,   0  // Yellow
		};

		// Use tightly packed data
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Generate a texture object
		glGenTextures(1, &textureId);

		// Bind the texture object
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Load the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			2, 2, 0,
			GL_RGB, GL_UNSIGNED_BYTE, pixels);

		// Set the filtering mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		return textureId;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static NAMESPACE_SimpleTexture2D::SimpleTexture2D* simpleTexture2D;
extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_SimpleTexture2DRenderer_00024Companion_nativeSurfaceCreate(
	JNIEnv * env, jobject thiz) {
	simpleTexture2D = new NAMESPACE_SimpleTexture2D::SimpleTexture2D();
	if (simpleTexture2D != nullptr) {
		simpleTexture2D->create();
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_SimpleTexture2DRenderer_00024Companion_nativeSurfaceChange(
	JNIEnv * env, jobject thiz, jint width, jint height) {
	if (simpleTexture2D != nullptr) {
		simpleTexture2D->change(width, height);
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_SimpleTexture2DRenderer_00024Companion_nativeDrawFrame(
	JNIEnv * env, jobject thiz) {
	if (simpleTexture2D != nullptr) {
		simpleTexture2D->draw();
	}
}