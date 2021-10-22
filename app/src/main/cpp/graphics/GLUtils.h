#ifndef OPEN_GL_LESSON_NATIVE_GL_UTILS_H_
#define OPEN_GL_LESSON_NATIVE_GL_UTILS_H_

#include <jni.h>
#include <GLES3/gl3.h>

class GLUtils {
public:
	/**
	 * Set Environment parameter
	 */
	static void setEnvAndAssetManager(JNIEnv* env, jobject assetManager);

	/**
	 *  Loads a file from assets/path into a char array.
	 */
	static char* openTextFile(const char* path);

	/**
	 * Loads a texture from assets/texture/<name>
	 */
	static GLuint loadTexture(const char* name);

	/**
	 * Create a program with the given vertex and framgent
	 * shader source code.
	 */
	static GLuint createProgram(const char** vertexSource, const char** fragmentSource);

	/**
	 * Current Time Millis
	 */
	static long currentTimeMillis();

	static GLfloat* generateCubeData(
		float* point1, float* point2, float* point3, float* point4,
		float* point5, float* point6, float* point7, float* point8,
		int elementsPerPoint);
};

#endif //OPEN_GL_LESSON_NATIVE_GL_UTILS_H_
