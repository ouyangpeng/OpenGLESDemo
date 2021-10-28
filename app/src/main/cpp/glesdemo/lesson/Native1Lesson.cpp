#include "Native1Lesson.h"
#include "../../utils/GLUtils.h"

static void checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}
// 顶点着色器
const char* VERTEX_SHADER =
"uniform mat4 u_MVPMatrix;      \n"                 // A constant representing the combined model/view/projection matrix.
"attribute vec4 a_Position;     \n"                 // Per-vertex position information we will pass in.
"attribute vec4 a_Color;        \n"                 // Per-vertex color information we will pass in.
"varying vec4 v_Color;          \n"                 // This will be passed into the fragment shader.
"void main()                    \n"                 // The entry point for our vertex shader.
"{                              \n"
"   v_Color = a_Color;          \n"                 // Pass the color through to the fragment shader.
"   gl_Position = u_MVPMatrix * a_Position; \n"     // gl_Position is a special variable used to store the final position.
"}                              \n";                // normalized screen coordinates.

// 片段着色器
const char* FRAGMENT_SHADER =
"precision mediump float;       \n"     // Set the default precision to medium. We don't need as high of a
"varying vec4 v_Color;          \n"     // This is the color from the vertex shader interpolated across the
"void main()                    \n"     // The entry point for our fragment shader.
"{                              \n"
"   gl_FragColor = v_Color;     \n"     // Pass the color directly through the pipeline.
"}                              \n";

// This triangle is red, green, and blue.
GLfloat triangle1VerticesData[] = {
	// X, Y, Z,
	// R, G, B, A
	-0.5f, -0.25f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	0.5f, -0.25f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 0.559016994f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f };

// This triangle is yellow, cyan, and magenta.
GLfloat triangle2VerticesData[] = {
	// X, Y, Z,
	// R, G, B, A
	-0.5f, -0.25f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f,

	0.5f, -0.25f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

	0.0f, 0.559016994f, 0.0f,
	1.0f, 0.0f, 1.0f, 1.0f };

// This triangle is white, gray, and black.
GLfloat triangle3VerticesData[] = {
	// X, Y, Z,
	// R, G, B, A
	-0.5f, -0.25f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,

	0.5f, -0.25f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f,

	0.0f, 0.559016994f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f };

Native1Lesson::Native1Lesson() {
	mModelMatrix = nullptr;
	mMVPMatrix = nullptr;
	mProjectionMatrix = nullptr;
	mViewMatrix = nullptr;
}

Native1Lesson::~Native1Lesson() {
	delete mModelMatrix;
	mModelMatrix = nullptr;

	delete mMVPMatrix;
	mMVPMatrix = nullptr;

	delete mProjectionMatrix;
	mProjectionMatrix = nullptr;

	delete mViewMatrix;
	mViewMatrix = nullptr;
}

void Native1Lesson::create() {
	GLUtils::printGLString("Version", GL_VERSION);
	GLUtils::printGLString("Vendor", GL_VENDOR);
	GLUtils::printGLString("Renderer", GL_RENDERER);
	GLUtils::printGLString("Extensions", GL_EXTENSIONS);

	mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
	if (!mProgram) {
		LOGD("Could not create program");
		return;
	}

	mModelMatrix = new Matrix();
	mMVPMatrix = new Matrix();

	// Position the eye in front of the origin.
	float eyeX = 0.0f;
	float eyeY = 0.0f;
	float eyeZ = 1.5f;

	// We are looking at the origin
	float centerX = 0.0f;
	float centerY = 0.0f;
	float centerZ = 0.0f;

	// Set our up vector.
	float upX = 0.0f;
	float upY = 1.0f;
	float upZ = 0.0f;

	// Set the view matrix.
	mViewMatrix = Matrix::newLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
}

void Native1Lesson::change(int width, int height) {

	glViewport(0, 0, width, height);

	// Create a new perspective projection matrix. The height will stay the same
	// while the width will vary as per aspect ratio.
	float ratio = (float)width / height;
	float left = -ratio;
	float right = ratio;
	float bottom = -1.0f;
	float top = 1.0f;
	float near = 1.0f;
	float far = 2.0f;

	mProjectionMatrix = Matrix::newFrustum(left, right, bottom, top, near, far);
}

void Native1Lesson::draw() {
	glClearColor(0.5F, 0.5F, 0.5F, 0.5F);
	glClear(GL_COLOR_BUFFER_BIT);
	checkGlError("glClear");

	glUseProgram(mProgram);

	// 返回统一变量的位置
	mMVPMatrixHandle = (GLuint)glGetUniformLocation(mProgram, "u_MVPMatrix");
	mPositionHandle = (GLuint)glGetAttribLocation(mProgram, "a_Position");
	mColorHandle = (GLuint)glGetAttribLocation(mProgram, "a_Color");

	long time = TimeUtils::currentTimeMillis() % 10000L;
	float angleInDegrees = (360.0f / 10000.0f) * ((int)time);

	// Draw the triangle facing straight on.
	mModelMatrix->identity();
	mModelMatrix->rotate(angleInDegrees, 0.0f, 0.0f, 1.0f);
	drawTriangle(triangle1VerticesData);

	// Draw one translated a bit down and rotated to be flat on the ground.
	mModelMatrix->identity();
	mModelMatrix->translate(0.0f, -1.0f, 0.0f);
	mModelMatrix->rotate(90.0f, 1.0f, 0.0f, 0.0f);
	mModelMatrix->rotate(angleInDegrees, 0.0f, 0.0f, 1.0f);
	drawTriangle(triangle2VerticesData);

	// Draw one translated a bit to the right and rotated to be facing to the left.
	mModelMatrix->identity();
	mModelMatrix->translate(1.0f, 0.0f, 0.0f);
	mModelMatrix->rotate(90.0f, 0.0f, 1.0f, 0.0f);
	mModelMatrix->rotate(angleInDegrees, 0.0f, 0.0f, 1.0f);
	drawTriangle(triangle3VerticesData);
}

void Native1Lesson::drawTriangle(GLfloat* verticesData) {

	glVertexAttribPointer(
		(GLuint)mPositionHandle,
		3,
		GL_FLOAT,
		GL_FALSE,
		4 * 7,
		verticesData
	);
	glEnableVertexAttribArray((GLuint)mPositionHandle);

	glVertexAttribPointer(
		(GLuint)mColorHandle,
		4,
		GL_FLOAT,
		GL_FALSE,
		4 * 7,
		verticesData + 3
	);
	glEnableVertexAttribArray((GLuint)mColorHandle);

	// model * view
	mMVPMatrix->multiply(*mViewMatrix, *mModelMatrix);

	// model * view * projection
	mMVPMatrix->multiply(*mProjectionMatrix, *mMVPMatrix);

	glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, mMVPMatrix->mData);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	checkGlError("glDrawArrays");
}


/// =======================================================

static Native1Lesson* native1Lesson;

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_lesson_LessonOneNativeRenderer_00024Companion_nativeSurfaceCreate(
		JNIEnv *env, jobject thiz) {

	if (native1Lesson) {
		delete native1Lesson;
		native1Lesson = nullptr;
	}
	native1Lesson = new Native1Lesson();
	native1Lesson->create();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_lesson_LessonOneNativeRenderer_00024Companion_nativeSurfaceChange(
		JNIEnv *env, jobject thiz, jint width, jint height) {
	if (native1Lesson != nullptr) {
		native1Lesson->change(width, height);
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_lesson_LessonOneNativeRenderer_00024Companion_nativeDrawFrame(JNIEnv *env,
																						jobject thiz) {
	if (native1Lesson != nullptr) {
		native1Lesson->draw();
	}
}