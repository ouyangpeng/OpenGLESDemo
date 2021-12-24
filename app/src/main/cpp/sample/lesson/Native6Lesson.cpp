#include "Native6Lesson.h"

void Native6Lesson::Create() {
    // Use culling to remove back face.
    glEnable(GL_CULL_FACE);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main Program
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile("vertex/vertex_shader_lesson_6.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile("fragment/fragment_shader_lesson_6.glsl");

    // Set program handles
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
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
    if (!mPointProgramHandle) {
        LOGD("Could not Create program")
        return;
    }

    // Load the texture
    mBrickDataHandle = GLUtils::loadTexture("texture/stone_wall_public_domain.png");
    glGenerateMipmap(GL_TEXTURE_2D);

    mGrassDataHandle = GLUtils::loadTexture("texture/noisy_grass_public_domain.png");
    glGenerateMipmap(GL_TEXTURE_2D);

    mLightModelMatrix = new Matrix();
    mModelMatrix = new Matrix();
    mMVPMatrix = new Matrix();
    mCurrentRotationMatrix = new Matrix();
    mAccumulatedRotationMatrix = new Matrix();

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

    if (mQueuedMinFilter != 0) {
        SetMinFilter(mQueuedMinFilter);
    }

    if (mQueuedMagFilter != 0) {
        SetMagFilter(mQueuedMagFilter);
    }
}

void Native6Lesson::Change(int width, int height) {

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

void Native6Lesson::Draw() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Do a compile rotation every 10 seconds;
    long time = TimeUtils::currentTimeMillis() % 10000L;
    long slowTime = TimeUtils::currentTimeMillis() % 100000L;
    float angleInDegrees = (180.0f / 100000.0f) * ((int) time);
    float slowAngleInDegrees = (180.0f / 1000000.0f) * ((int) slowTime);

    // Set out pre-vertex lighting program.
    glUseProgram(m_ProgramObj);

    // Set program handles for cube drawing.
    mMVPMatrixHandle = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    mMVMatrixHandle = glGetUniformLocation(m_ProgramObj, "u_MVMatrix");
    mLightPosHandle = glGetUniformLocation(m_ProgramObj, "u_LightPos");
    mTextureUniformHandle = glGetUniformLocation(m_ProgramObj, "u_Texture");

    // Calculate position of the light
    // Rotate and then push into the distance.
    mLightModelMatrix->identity();
    mLightModelMatrix->translate(0, 0, -2);
    mLightModelMatrix->rotate(angleInDegrees, 0, 1, 0);
    mLightModelMatrix->translate(0, 0, 3.5);

    Matrix::multiplyMV(mLightPosInWorldSpace, mLightModelMatrix->mData, mLightPosInModelSpace);
    Matrix::multiplyMV(mLightPosInEyeSpace, mViewMatrix->mData, mLightPosInWorldSpace);

    // Draw a cube.
    // Translate the cube into the screen.
    mModelMatrix->identity();
    mModelMatrix->translate(0.0f, 0.8f, -3.5f);
//    mModelMatrix->rotate(angleInDegrees, 1.0f, 1.0f, 1.0f);

    // Set a matrix that contains the current rotation.
    mCurrentRotationMatrix->identity();
    mCurrentRotationMatrix->rotate(mDeltaX, 0.0f, 1.0f, 0.0f);
    mCurrentRotationMatrix->rotate(mDeltaY, 1.0f, 0.0f, 0.0f);
    mDeltaX = 0.0f;
    mDeltaY = 0.0f;

    Matrix tempMatrix;

    // Multiply the current rotation by the accumulated rotation, and then set the accumulated rotation to the result.
    tempMatrix.identity();
    tempMatrix.multiply(*mCurrentRotationMatrix, *mAccumulatedRotationMatrix);
    mAccumulatedRotationMatrix->loadWith(tempMatrix);

    // Rotate the cube taking the overall rotation into account.
    tempMatrix.identity();
    tempMatrix.multiply(*mModelMatrix, *mAccumulatedRotationMatrix);
    mModelMatrix->loadWith(tempMatrix);

    // Set the active texture unit to texture unit 0.
    glActiveTexture(GL_TEXTURE0);

    // Bind the texture to this unit.
    glBindTexture(GL_TEXTURE_2D, mBrickDataHandle);

    // Tell the texture uniform sampler to use this texture in the shader by binding to texture unit 0.
    glUniform1i(mTextureUniformHandle, 0);

    drawCube();

    // Draw a plane
    mModelMatrix->identity();
    mModelMatrix->translate(0.0f, -2.0f, -5.0f);
    mModelMatrix->scale(25.0f, 1.0f, 25.0f);
    mModelMatrix->rotate(slowAngleInDegrees, 0.0f, 1.0f, 0.0f);

    // Set the active texture unit to texture unit 0.
    glActiveTexture(GL_TEXTURE0);

    // Bind the texture to this unit.
    glBindTexture(GL_TEXTURE_2D, mGrassDataHandle);

    // Tell the texture uniform sampler to use this texture in the shader by binding to texture unit 0.
    glUniform1i(mTextureUniformHandle, 0);

    drawCube();

    // Draw a point to indicate the light
    glUseProgram(mPointProgramHandle);
    drawLight();
}

void Native6Lesson::SetMinFilter(int filter) {
    if (mBrickDataHandle != 0 && mGrassDataHandle != 0) {
        glBindTexture(GL_TEXTURE_2D, mBrickDataHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glBindTexture(GL_TEXTURE_2D, mGrassDataHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    } else {
        mQueuedMinFilter = filter;
    }
}

void Native6Lesson::SetMagFilter(int filter) {
    if (mBrickDataHandle != 0 && mGrassDataHandle != 0) {
        glBindTexture(GL_TEXTURE_2D, mBrickDataHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glBindTexture(GL_TEXTURE_2D, mGrassDataHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    } else {
        mQueuedMagFilter = filter;
    }
}

void Native6Lesson::drawCube() {

    // Pass in the position info
    glVertexAttribPointer(
            NATIVE_LESSON_SIX_ATTRIB_LOCATION_POS,
            POSITION_DATA_SIZE,
            GL_FLOAT,
            GL_FALSE,
            0,
            CUBE_POSITION_DATA
    );
    glEnableVertexAttribArray(NATIVE_LESSON_SIX_ATTRIB_LOCATION_POS);

    // Pass in the texture coordinate information
    glVertexAttribPointer(
            NATIVE_LESSON_SIX_ATTRIB_LOCATION_TEX_COORDINATE,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            CUBE_TEXTURE_COORDINATE_DATA
    );
    glEnableVertexAttribArray(NATIVE_LESSON_SIX_ATTRIB_LOCATION_TEX_COORDINATE);

    // Pass in the normal information
    glVertexAttribPointer(
            NATIVE_LESSON_SIX_ATTRIB_LOCATION_NORMAL,
            NORMAL_DATA_SIZE,
            GL_FLOAT,
            GL_FALSE,
            0,
            CUBE_NORMAL_DATA
    );
    glEnableVertexAttribArray(NATIVE_LESSON_SIX_ATTRIB_LOCATION_NORMAL);

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

void Native6Lesson::drawLight() {

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

void Native6Lesson::SetDelta(float x, float y) {
    mDeltaX += x;
    mDeltaY += y;
}

void Native6Lesson::Shutdown() {
    delete mModelMatrix;
    mModelMatrix = nullptr;

    delete mViewMatrix;
    mViewMatrix = nullptr;

    delete mProjectionMatrix;
    mProjectionMatrix = nullptr;

    delete mMVPMatrix;
    mMVPMatrix = nullptr;

    delete mAccumulatedRotationMatrix;
    mAccumulatedRotationMatrix = nullptr;

    delete mCurrentRotationMatrix;
    mCurrentRotationMatrix = nullptr;

    delete mLightModelMatrix;
    mLightModelMatrix = nullptr;

    // Delete texture object
    glDeleteTextures ( 1, &mBrickDataHandle );
    glDeleteTextures ( 1, &mGrassDataHandle );

    // Delete program object
    GLUtils::DeleteProgram(m_ProgramObj);
    GLUtils::DeleteProgram(mPointProgramHandle);
}
