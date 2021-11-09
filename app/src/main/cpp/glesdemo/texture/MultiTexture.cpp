//
// Created by OuyangPeng on 2021/10/25.
//

#include "MultiTexture.h"

MultiTexture::MultiTexture() {

}

MultiTexture::~MultiTexture() {

}

void MultiTexture::create() {
	GLUtils::printGLString("Version", GL_VERSION);
	GLUtils::printGLString("Vendor", GL_VENDOR);
	GLUtils::printGLString("Renderer", GL_RENDERER);
	GLUtils::printGLString("Extensions", GL_EXTENSIONS);

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

	const char* FRAGMENT_SHADER_TRIANGLE =
		"#version 300 es                                     \n"
		"precision mediump float;                            \n"
		"in vec2 v_texCoord;                                 \n"
		"layout(location = 0) out vec4 outColor;             \n"
		"uniform sampler2D s_baseMap;                        \n"
		"uniform sampler2D s_lightMap;                       \n"
		"void main()                                         \n"
		"{                                                   \n"
		"  vec4 baseColor;                                   \n"
		"  vec4 lightColor;                                  \n"
		"                                                    \n"
		"  baseColor = texture( s_baseMap, v_texCoord );     \n"
		"  lightColor = texture( s_lightMap, v_texCoord );   \n"
		"  outColor = baseColor * (lightColor + 0.25);       \n"
		"}                                                   \n";

	programObject = GLUtils::createProgram(&VERTEX_SHADER_TRIANGLE, &FRAGMENT_SHADER_TRIANGLE);
	if (!programObject) {
		LOGD("Could not create program");
		return;
	}

	// Get the sampler location
	baseMapLoc = glGetUniformLocation(programObject, "s_baseMap");
	lightMapLoc = glGetUniformLocation(programObject, "s_lightMap");

	// Load the textures
	// 从assets目录下 取出对应的Texture
	baseMapTexId = GLUtils::loadTexture("texture/basemap.png");
	lightMapTexId = GLUtils::loadTexture("texture/lightmap.png");

	if (baseMapTexId == 0 || lightMapTexId == 0)
	{
		return;
	}

	// 设置清除颜色
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void MultiTexture::change(int width, int height) {
	mWidth = width;
	mHeight = height;
	LOGD("change() width = %d , height = %d\n", width, height);
	// Set the viewport
	glViewport(0, 0, mWidth, mHeight);
}

void MultiTexture::draw() {
	GLfloat vVertices[] = {
			-0.5f,  0.5f, 0.0f,  // Position 0
			0.0f,  0.0f,        // TexCoord 0

			-0.5f, -0.5f, 0.0f,  // Position 1
			0.0f,  1.0f,        // TexCoord 1

			0.5f, -0.5f, 0.0f,  // Position 2
			1.0f,  1.0f,        // TexCoord 2

			0.5f,  0.5f, 0.0f,  // Position 3
			1.0f,  0.0f         // TexCoord 3
	};
	GLushort indices[] = { 0, 1, 2, 0, 2, 3 };


	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	glUseProgram(programObject);

	// Load the vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT,
		GL_FALSE, 5 * sizeof(GLfloat), vVertices);
	// Load the texture coordinate
	glVertexAttribPointer(1, 2, GL_FLOAT,
		GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Bind the base map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, baseMapTexId);

	// Set the base map sampler to texture unit to 0
	glUniform1i(baseMapLoc, 0);

	// Bind the light map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightMapTexId);

	// Set the light map sampler to texture unit 1
	glUniform1i(lightMapLoc, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void MultiTexture::shutdown() {
	// Delete texture object
	glDeleteTextures(1, &baseMapTexId);
	glDeleteTextures(1, &lightMapTexId);

	// Delete program object
	glDeleteProgram(programObject);
}


//////////////////////////////////////////////////////////////////////////////////////
MultiTexture* multiTexture;

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_MultiTextureRenderer_nativeSurfaceCreate(
	JNIEnv * env, jobject thiz, jobject assetManager) {
	// 初始化设置assetManager  一定要记得初始化，否则会报空指针异常
	GLUtils::setEnvAndAssetManager(env, assetManager);

	if (multiTexture) {
		delete multiTexture;
		multiTexture = nullptr;
	}
	multiTexture = new MultiTexture();
	multiTexture->create();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_MultiTextureRenderer_nativeSurfaceChange(
	JNIEnv * env, jobject thiz, jint width, jint height) {
	if (multiTexture != nullptr) {
		multiTexture->change(width, height);
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_MultiTextureRenderer_nativeDrawFrame(JNIEnv * env,
	jobject thiz) {
	if (multiTexture != nullptr) {
		multiTexture->draw();
	}
}