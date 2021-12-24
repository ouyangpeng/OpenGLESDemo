#include "Native4Lesson.h"

Native4Lesson::Native4Lesson() {
    m_Width = 0;
    m_Height = 0;

    mViewMatrix = nullptr;
    mModelMatrix = nullptr;
    mProjectionMatrix = nullptr;
    mMVPMatrix = nullptr;
    mLightModelMatrix = nullptr;

    mMVPMatrixHandle = 0;
    mMVMatrixHandle = 0;
    mLightPosHandle = 0;

    mPerVertexProgramHandle = 0;
    mPointProgramHandle = 0;

    mLightPosInModelSpace[0] = 0.0f;
    mLightPosInModelSpace[1] = 0.0f;
    mLightPosInModelSpace[2] = 0.0f;
    mLightPosInModelSpace[3] = 1.0f;

    mLightPosInWorldSpace[0] = 0.0f;
    mLightPosInWorldSpace[1] = 0.0f;
    mLightPosInWorldSpace[2] = 0.0f;
    mLightPosInWorldSpace[3] = 0.0f;

    mLightPosInEyeSpace[0] = 0.0f;
    mLightPosInEyeSpace[1] = 0.0f;
    mLightPosInEyeSpace[2] = 0.0f;
    mLightPosInEyeSpace[3] = 0.0f;
}

Native4Lesson::~Native4Lesson() = default;

void Native4Lesson::Create() {
    LOGD("Native4Lesson Create")

    // Use culling to remove back face.
    glEnable(GL_CULL_FACE);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main Program
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile("vertex/vertex_shader_lesson_4.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile("fragment/fragment_shader_lesson_4.glsl");

    // Set program handles
    mPerVertexProgramHandle = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!mPerVertexProgramHandle) {
        LOGD("Could not Create program")
        return;
    }

    const char *POINT_VERTEX_SHADER_CODE = GLUtils::openTextFile(
            "vertex/vertex_shader_lesson_point.glsl");
    const char *POINT_FRAGMENT_SHADER_CODE = GLUtils::openTextFile(
            "fragment/fragment_shader_lesson_point.glsl");
    // Set Point program handle
    mPointProgramHandle = GLUtils::createProgram(&POINT_VERTEX_SHADER_CODE,
                                                 &POINT_FRAGMENT_SHADER_CODE);

    mTextureDataHandle = GLUtils::loadTexture("texture/bumpy_bricks_public_domain.jpg");

    mLightModelMatrix = new Matrix();
    mModelMatrix = new Matrix();
    mMVPMatrix = new Matrix();

    // Position the eye in front of the origin.
    float eyeX = 0.0f;
    float eyeY = 0.0f;
    float eyeZ = 1.5f;

    // We are looking at the origin
    float centerX = 0.0f;
    float centerY = 0.0f;
    float centerZ = -5.0f;

    // Set our up vector.
    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;

    // Set the view matrix.
    mViewMatrix = Matrix::newLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
}

void Native4Lesson::Change(int width, int height) {

    m_Width = width;
    m_Height = height;

    glViewport(0, 0, m_Width, m_Height);

    // Create a new perspective projection matrix. The height will stay the same
    // while the width will vary as per aspect ratio.
    float ratio = (float) width / height;
    float left = -ratio;
    float right = ratio;
    float bottom = -1.0f;
    float top = 1.0f;
    float near = 1.0f;
    float far = 10.0f;

    mProjectionMatrix = Matrix::newFrustum(left, right, bottom, top, near, far);
}

void Native4Lesson::Draw() {
// Set the OpenGL viewport to same size as the surface.

    glClearColor(0, 0, 0, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Do a compile rotation every 10 seconds;
    long time = TimeUtils::currentTimeMillis() % 10000L;
    float angleInDegrees = (360.0f / 10000.0f) * ((int) time);

    // Set out pre-vertex lighting program.
    glUseProgram(mPerVertexProgramHandle);

    // Set program handle for cube drawing.
    mMVPMatrixHandle = (GLuint) glGetUniformLocation(mPerVertexProgramHandle, "u_MVPMatrix");
    mMVMatrixHandle = (GLuint) glGetUniformLocation(mPerVertexProgramHandle, "u_MVMatrix");

    mLightPosHandle = (GLuint) glGetUniformLocation(mPerVertexProgramHandle, "u_LightPos");
    mTextureUniformHandle = (GLuint) glGetUniformLocation(mPerVertexProgramHandle, "u_Texture");

    // Set the active texture unit to texture unit 0.
    glActiveTexture(GL_TEXTURE0);

    // Bind the texture to this unit
    glBindTexture(GL_TEXTURE_2D, mTextureDataHandle);

    // Tell the texture uniform sampler to use the texture
    // in the shader by binding to texture unit 0.
    glUniform1i(mTextureUniformHandle, 0);


    // Calculate position of the light
    // Rotate and then push into the distance.
    mLightModelMatrix->identity();
    mLightModelMatrix->translate(0, 0, -5);
    mLightModelMatrix->rotate(angleInDegrees, 0, 1, 0);
    mLightModelMatrix->translate(0, 0, 2);

    Matrix::multiplyMV(mLightPosInWorldSpace, mLightModelMatrix->mData, mLightPosInModelSpace);
    Matrix::multiplyMV(mLightPosInEyeSpace, mViewMatrix->mData, mLightPosInWorldSpace);

    // right
    mModelMatrix->identity();
    mModelMatrix->translate(4.0f, 0.0f, -7.0f);
    mModelMatrix->rotate(angleInDegrees, 1.0f, 0.0f, 0.0f);
    drawCube();

    // left
    mModelMatrix->identity();
    mModelMatrix->translate(-4.0f, 0.0f, -7.0f);
    mModelMatrix->rotate(angleInDegrees, 0.0f, 1.0f, 0.0f);
    drawCube();

    // top
    mModelMatrix->identity();
    mModelMatrix->translate(0.0f, 4.0f, -7.0f);
    mModelMatrix->rotate(angleInDegrees, 0.0f, 1.0f, 0.0f);
    drawCube();

    // bottom
    mModelMatrix->identity();
    mModelMatrix->translate(0.0f, -4.0f, -7.0f);
    mModelMatrix->rotate(angleInDegrees, 0.0f, 1.0f, 0.0f);
    drawCube();

    // center
    mModelMatrix->identity();
    mModelMatrix->translate(0.0f, 0.0f, -5.0f);
    mModelMatrix->rotate(angleInDegrees, 1.0f, 1.0f, 1.0f);
    drawCube();



    // Draw a pint to indicate the light
    glUseProgram(mPointProgramHandle);
    drawLight();
}

void Native4Lesson::drawCube() {
    // Pass in the position info
    glVertexAttribPointer(
            NATIVE_LESSON_ATTRIB_LOCATION_POS,
            NATIVE_LESSON_POSITION_DATA_SIZE,
            GL_FLOAT,
            GL_FALSE,
            0,
            CUBE_POSITION_DATA
    );
    glEnableVertexAttribArray(NATIVE_LESSON_ATTRIB_LOCATION_POS);

    // Pass in the color info
    glVertexAttribPointer(
            NATIVE_LESSON_ATTRIB_LOCATION_COLOR,
            NATIVE_LESSON_COLOR_DATA_SIZE,
            GL_FLOAT,
            GL_FALSE,
            0,
            CUBE_COLOR_DATA
    );
    glEnableVertexAttribArray(NATIVE_LESSON_ATTRIB_LOCATION_COLOR);

    // Pass in the normal information
    glVertexAttribPointer(
            NATIVE_LESSON_ATTRIB_LOCATION_NORMAL,
            NATIVE_LESSON_NORMAL_DATA_SIZE,
            GL_FLOAT,
            GL_FALSE,
            0,
            CUBE_NORMAL_DATA
    );
    glEnableVertexAttribArray(NATIVE_LESSON_ATTRIB_LOCATION_NORMAL);

    // Pass in the texture coordinate information
    glVertexAttribPointer(
            NATIVE_LESSON_ATTRIB_LOCATION_TEXTURE_COORDINATE,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            CUBE_TEXTURE_COORDINATE_DATA
    );
    glEnableVertexAttribArray(NATIVE_LESSON_ATTRIB_LOCATION_TEXTURE_COORDINATE);


    // This multiplies the view by the model matrix
    // and stores the result the MVP matrix.
    // which currently contains model * view
    mMVPMatrix->multiply(*mViewMatrix, *mModelMatrix);

    // Pass in the model view matrix
    glUniformMatrix4fv(
            mMVMatrixHandle,
            1,
            GL_FALSE,
            mMVPMatrix->mData
    );

    // This multiplies the model view matrix by the projection matrix
    // and stores the result in the MVP matrix.
    // which no contains model * view * projection
    mMVPMatrix->multiply(*mProjectionMatrix, *mMVPMatrix);

    // Pass in the model view projection matrix
    glUniformMatrix4fv(
            mMVPMatrixHandle,
            1,
            GL_FALSE,
            mMVPMatrix->mData
    );

    // Pass in the light position in eye space
    glUniform3f(mLightPosHandle,
                mLightPosInEyeSpace[0],
                mLightPosInEyeSpace[1],
                mLightPosInEyeSpace[2]
    );

    // Draw the cube
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Native4Lesson::drawLight() {

    GLint pointMVPMatrixHandle = glGetUniformLocation(mPointProgramHandle, "u_MVPMatrix");

    // Pass in the position
    glVertexAttrib3f(
            NATIVE_LESSON_ATTRIB_LOCATION_POINT_POS,
            mLightPosInModelSpace[0],
            mLightPosInModelSpace[1],
            mLightPosInModelSpace[2]);

    // Since we are not using a buffer object,
    // disable vertex arrays for the attribute
    glDisableVertexAttribArray(NATIVE_LESSON_ATTRIB_LOCATION_POINT_POS);

    // Pass in the transformation matrix.
    mMVPMatrix->identity();
    mMVPMatrix->multiply(*mViewMatrix, *mLightModelMatrix);
    mMVPMatrix->multiply(*mProjectionMatrix, *mMVPMatrix);

    glUniformMatrix4fv(
            pointMVPMatrixHandle,
            1,
            GL_FALSE,
            mMVPMatrix->mData
    );

    glDrawArrays(GL_POINTS, 0, 1);
}

void Native4Lesson::Shutdown() {
    delete mModelMatrix;
    mModelMatrix = nullptr;
    delete mViewMatrix;
    mViewMatrix = nullptr;
    delete mProjectionMatrix;
    mProjectionMatrix = nullptr;
    delete mLightModelMatrix;
    mLightModelMatrix = nullptr;

    // Delete program object
    GLUtils::DeleteProgram(mPerVertexProgramHandle);
    GLUtils::DeleteProgram(mPointProgramHandle);
}
