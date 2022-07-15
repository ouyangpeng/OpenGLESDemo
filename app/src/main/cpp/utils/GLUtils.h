#ifndef OPEN_GL_LESSON_NATIVE_GL_UTILS_H_
#define OPEN_GL_LESSON_NATIVE_GL_UTILS_H_

#include <jni.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <android/asset_manager_jni.h>
#include <string>

#include <Matrix.h>
#include <LogUtils.h>
#include <time/TimeUtils.h>
#include <esShapes.h>
#include <esTransform.h>

#include <GLBaseSample.h>

#include <glm/detail/type_mat4x4.hpp>

#define MATH_PI 3.1415926535897932384626433832802

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

	static GLuint loadTgaTexture(const char *fileName);
	/**
	 * Create a program with the given vertex and framgent
	 * shader source code.
	 */
	static GLuint createProgram(const char** vertexSource, const char** fragmentSource);

	static void DeleteProgram(GLuint &program);

	static void checkGlError(const char* pGLOperation);

	static void printGLString(const char *name, GLenum s) {
		const char *v = (const char *) glGetString(s);
		LOGI("GL %s = %s \n", name, v);
	}

	// Print some OpenGL info
	static void printGLInfo(){
        LOGI("===== Here are some information of your device =====");
		printGLString("Version", GL_VERSION);
		printGLString("GLSL Version", GL_SHADING_LANGUAGE_VERSION);
		printGLString("Vendor", GL_VENDOR);
		printGLString("Renderer", GL_RENDERER);
		printGLString("Extensions", GL_EXTENSIONS);

		GLint iParam[2];
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, iParam);
		LOGI("Max Vertex Uniform Vectors: %d", iParam[0]);
		
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, iParam);
		LOGI("Max Fragment Uniform Vectors : %d", iParam[0]);

		glGetIntegerv(GL_MAX_VARYING_VECTORS, iParam);
		LOGI("Max Varying Vectors: %d", iParam[0]);

		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, iParam);
		LOGI("Max Texture Image Units : %d", iParam[0]);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, iParam);
		LOGI("Max Combined Texture Image Units : %d", iParam[0]);

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, iParam);
		LOGI("Max Texture Size : %d", iParam[0]);

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, iParam);
		LOGI("Max Vertex Attribs: %d", iParam[0]);

		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, iParam);
		LOGI("Max Viewport Dims : [%d, %d]", iParam[0], iParam[1]);

		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, iParam);
		LOGI("Max Render Buffer Size: %d", iParam[0]);

		glGetIntegerv(GL_STENCIL_BITS, iParam);
		LOGI("Stencil Buffer Bits : %d", iParam[0]);

		glGetIntegerv(GL_ALIASED_POINT_SIZE_RANGE, iParam);
		LOGI("Point Size Range: [%d, %d]", iParam[0], iParam[1]);

		glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, iParam);
		LOGI("Line Width Range: [%d, %d]", iParam[0], iParam[1]);

		glGetIntegerv(GL_DEPTH_BITS, iParam);
		LOGI("Depth Bits: %d", iParam[0]);

        LOGI("===== Information end =====");
	}

	static void setBool(GLuint programId, const std::string &name, bool value) {
		glUniform1i(glGetUniformLocation(programId, name.c_str()), (int) value);
	}

	static void setInt(GLuint programId, const std::string &name, int value) {
		glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
	}

	static void setFloat(GLuint programId, const std::string &name, float value) {
		glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
	}

	static void setVec2(GLuint programId, const std::string &name, const glm::vec2 &value) {
		glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
	}

	static void setVec2(GLuint programId, const std::string &name, float x, float y) {
		glUniform2f(glGetUniformLocation(programId, name.c_str()), x, y);
	}

	static void setVec3(GLuint programId, const std::string &name, const glm::vec3 &value) {
		glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
	}

	static void setVec3(GLuint programId, const std::string &name, float x, float y, float z) {
		glUniform3f(glGetUniformLocation(programId, name.c_str()), x, y, z);
	}

	static void setVec4(GLuint programId, const std::string &name, const glm::vec4 &value) {
		glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
	}

	static void setVec4(GLuint programId, const std::string &name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(programId, name.c_str()), x, y, z, w);
	}

	static void setMat2(GLuint programId, const std::string &name, const glm::mat2 &mat) {
		glUniformMatrix2fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	static void setMat3(GLuint programId, const std::string &name, const glm::mat3 &mat) {
		glUniformMatrix3fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	static void setMat4(GLuint programId,  const std::string &name, const glm::mat4 &mat){
		glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	static glm::vec3 texCoordToVertexCoord(glm::vec2 &texCoord) {
		return glm::vec3(2 * texCoord.x - 1, 1 - 2 * texCoord.y, 0);
	}
};

#endif //OPEN_GL_LESSON_NATIVE_GL_UTILS_H_
