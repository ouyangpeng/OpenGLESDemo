#include <cstring>
#include "NativeTriangleMapBuffers.h"
#include "NativeTriangleVertextArrayObject.h"


// 可以参考这篇讲解： https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
// NDK OpenGL ES 3.0 开发（四）：VBO、EBO 和 VAO https://blog.csdn.net/Kennethdroid/article/details/98088890

//VBO 和 EBO
//VBO（Vertex Buffer Object）是指顶点缓冲区对象，
//而 EBO（Element Buffer Object）是指图元索引缓冲区对象，VAO 和 EBO 实际上是对同一类 Buffer 按照用途的不同称呼。

// OpenGLES2.0 编程中，用于绘制的顶点数组数据首先保存在 CPU 内存，
// 在调用 glDrawArrays 或者 glDrawElements 等进行绘制时，需要将顶点数组数据从 CPU 内存拷贝到显存。
// 但是很多时候我们没必要每次绘制的时候都去进行内存拷贝，如果可以在显存中缓存这些数据，就可以在很大程度上降低内存拷贝带来的开销。

// OpenGLES3.0 VBO 和 EBO 的出现就是为了解决这个问题。
// VBO 和 EBO 的作用是在显存中提前开辟好一块内存，用于缓存顶点数据或者图元索引数据，
// 从而避免每次绘制时的 CPU 与 GPU 之间的内存拷贝，可以提升渲染性能，降低内存带宽和功耗。
// OpenGLES3.0 支持两类缓冲区对象：顶点数组缓冲区对象、图元索引缓冲区对象。
//    GL_ARRAY_BUFFER 标志指定的缓冲区对象用于保存顶点数组，
//    GL_ELEMENT_ARRAY_BUFFER 标志指定的缓存区对象用于保存图元索引。
// VBO 可以参考图片：docs/vertex_attribute_pointer_interleaved


// VAO（Vertex Array Object）是指顶点数组对象，VAO 的主要作用是用于管理 VBO 或 EBO ，
// 减少 glBindBuffer 、glEnableVertexAttribArray、 glVertexAttribPointer 这些调用操作，高效地实现在顶点数组配置之间切换。
// VAO 与 VBO、EBO之间的关系  可以参考图片： docs/vertex_array_objects.png 和 docs/vertex_array_objects_ebo.png


// 3 vertices, with (x,y,z) ,(r, g, b, a)  per-vertex
static GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
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
static GLushort indices[3] = {0, 1, 2};

void NativeTriangleVAO::Create() {
    GLUtils::printGLInfo();

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_hello_triangle2.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle2.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }

    //  Generate VBO Ids and load the VBOs with data
    // 创建 2 个 VBO（EBO 实际上跟 VBO 一样，只是按照用途的另一种称呼）
    glGenBuffers(2, vboIds);

    // 绑定第一个 VBO，拷贝顶点数组到显存
    // GL_STATIC_DRAW 标志标识缓冲区对象数据被修改一次，使用多次，用于绘制。
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定第二个 VBO（EBO），拷贝图元索引数据到显存
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    // 创建一个VAO
    glGenVertexArrays(1, &vaoId);

    // Bind the VAO and then setup the vertex attributes
    // 绑定VAO之后，操作 VBO ，当前 VAO 会记录 VBO 的操作
    // 要想使用VAO，要做的只是使用glBindVertexArray绑定VAO。
    // 从绑定之后起，我们应该绑定和配置对应的VBO和属性指针，之后解绑VAO供之后使用。
    // 当我们打算绘制一个物体的时候，我们只要在绘制物体前简单地把VAO绑定到希望使用的设定上就行了。
    glBindVertexArray(vaoId);

    // 把顶点数组复制到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);

    // 设置顶点属性指针
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
                          GL_FLOAT, GL_FALSE, VERTEX_STRIDE, nullptr);

    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
                          GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
                          (const void *) (VERTEX_POS_SIZE * sizeof(GLfloat)));

    // Reset to the default VAO
    glBindVertexArray(0);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NativeTriangleVAO::Draw() {
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
    glUseProgram(m_ProgramObj);

    // Bind the VAO
    glBindVertexArray(vaoId);

    // Draw with the VAO settings
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

    // Return to the default VAO
    glBindVertexArray(0);
}

void NativeTriangleVAO::Shutdown() {
    GLBaseSample::Shutdown();

    glDeleteBuffers(2, &vboIds[0]);
    glDeleteVertexArrays(1, &vaoId);
}