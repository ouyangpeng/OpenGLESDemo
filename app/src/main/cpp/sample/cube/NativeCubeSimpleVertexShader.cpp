#include <cstring>
#include <malloc.h>

#include "NativeCubeSimpleVertexShader.h"

void NativeCubeSimpleVertexShader::create() {
    GLUtils::printGLInfo();

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_cube.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle2.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }


    //    3D物体从建模到最终显示到屏幕上面要经历以下几个阶段：
    //
    //    对象空间(Object Space)
    //    世界空间(World Space)
    //    照相机空间(Camera Space/Eye Space)
    //    裁剪空间(Clipping Space)
    //    设备空间（normalized device space）
    //    视口空间(Viewport)
    //
    //    从对象空间到世界空间的变换通常叫做Model-To-World变换，从世界空间到照相机空间的变换叫做World-To-View变换，
    //    而从照相机空间到裁剪空间的变换叫做View-To-Projection。合起来，就是我们常常提到的MVP变换。

    //  mvp矩阵即模型矩阵，观察矩阵和投影矩阵。
    //  模型矩阵用来从模型坐标系变换到世界坐标系中。
    //  观察变换就是从世界坐标系下变换到观察坐标系下，就是在世界坐标系下设置个观察点（相机位置以及相机方向），然后把世界坐标系下的坐标变换到相机空间。

    // Get the uniform locations
    mvpLoc = glGetUniformLocation(mProgram, "u_mvpMatrix");

    // Generate the vertex data
    numIndices = esGenCube(1.0, &vertices, nullptr, nullptr, &indices);

    // Starting roating angle for cube
    angle = 45.0f;

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NativeCubeSimpleVertexShader::draw() {
    // 每次绘制之前先update一下
    update(getDeltaTime());

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(mProgram);

    // Load the vertex position
    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT,
                          GL_FALSE, 3 * sizeof(GLfloat), vertices);
    glEnableVertexAttribArray(VERTEX_POS_INDX);

    // Set the vertex color to red
    glVertexAttrib4f(VERTEX_COLOR_INDX, 1.0f, 0.0f, 0.0f, 1.0f);

    // Load the MVP matrix
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (GLfloat *) &mvpMatrix.m[0][0]);

    // Draw the cube
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);
}

float NativeCubeSimpleVertexShader::getDeltaTime() {
    if (mLastTime == 0) {
        mLastTime = TimeUtils::currentTimeMillis();
    }
    long currentTime = TimeUtils::currentTimeMillis();
    long elapsedTime = currentTime - mLastTime;
    float deltaTime = (float) elapsedTime / 1000.0f;
    mLastTime = currentTime;
    return deltaTime;
}

void NativeCubeSimpleVertexShader::update(float deltaTime) {
    ESMatrix perspective;
    ESMatrix modelview;
    float aspect;

    // Compute a rotation angle based on time to rotate the cube
    // 根据旋转立方体的时间计算旋转角度
    angle += (deltaTime * 40.0f);
    if (angle >= 360.0f) {
        angle -= 360.0f;
    }

    // Compute the window aspect ratio
    // 计算窗口纵横比
    aspect = (GLfloat) mWidth / (GLfloat) mHeight;

    // Generate a perspective matrix with a 60 degree FOV
    // and near and far clip planes at 1.0 and 20.0
    esMatrixLoadIdentity(&perspective);
    esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);

    // Generate a model view matrix to rotate/translate the cube
    // 在modelview矩阵中加载一个单位矩阵
    esMatrixLoadIdentity(&modelview);

    // Translate away from the viewer
    // 单位矩阵结合一个平移，使物体远离观看者
    esTranslate(&modelview, 0.0, 0.0, -2.0);

    // Rotate the cube
    // 对modelview矩阵进行一次旋转，使物体饶向量(1.0,0.0,1.0) 以根据时间更新的角度连续旋转物体
    esRotate(&modelview, angle, 1.0, 0.0, 1.0);

    // Compute the final MVP by multiplying the
    // modevleiw and perspective matrices together
    // 计算MVP矩阵--- 模型-视图矩阵和投影矩阵的乘积
    esMatrixMultiply(&mvpMatrix, &modelview, &perspective);
}

void NativeCubeSimpleVertexShader::shutdown() {
    if (vertices != nullptr) {
        free(vertices);
    }

    if (indices != nullptr) {
        free(indices);
    }

    // Delete program object
    GLUtils::DeleteProgram(mProgram);
}
