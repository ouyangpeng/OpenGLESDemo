#include "NativeTriangle3.h"

// 可以参考这篇讲解： https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/

// 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
static GLfloat vertexPos[3 * VERTEX_POS_SIZE] =
        {
                // 逆时针 三个顶点
                0.0f, 0.5f, 0.0f,            // 上角
                -0.5f, -0.5f, 0.0f,          // 左下角
                0.5f, -0.5f, 0.0f            // 右下角
        };

// 设置顶点的颜色值
static GLfloat color[4 * VERTEX_COLOR_SIZE] =
        {
                1.0f, 0.0f, 0.0f, 1.0f,   // c0
                0.0f, 1.0f, 0.0f, 1.0f,   // c1
                0.0f, 0.0f, 1.0f, 1.0f    // c2
        };

static GLint vtxStrides[2] =
        {
                VERTEX_POS_SIZE * sizeof(GLfloat),
                VERTEX_COLOR_SIZE * sizeof(GLfloat)
        };

// Index buffer data
// 注意索引从0开始!
static GLushort indices[3] = {
        0, 1, 2
};

static GLfloat *vtxBuf[2] = {vertexPos, color};


void NativeTriangle3::create() {
    GLUtils::printGLInfo();

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_hello_triangle2.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle2.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }

    vboIds[0] = 0;
    vboIds[1] = 0;
    vboIds[2] = 0;

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NativeTriangle3::draw() {
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
    glUseProgram(mProgram);

    if (vboIds[0] == 0 && vboIds[1] == 0 && vboIds[2] == 0) {
        // Only allocate on the first draw

        // 使用glGenBuffers函数生成3个VBO对象
        glGenBuffers(3, vboIds);

        // OpenGL有很多缓冲对象类型，顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER。
        // OpenGL允许我们同时绑定多个缓冲，只要它们是不同的缓冲类型。
        // 我们可以使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
        // 从这一刻起，我们使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)。

        // VBO 0 是 缓冲 顶点
        // 复制顶点数组到缓冲中供OpenGL使用
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
        /**
         * 然后我们可以调用glBufferData函数，它会把之前定义的顶点数据复制到缓冲的内存中。
         * glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数。
         *  它的第一个参数是目标缓冲的类型：顶点缓冲对象当前绑定到GL_ARRAY_BUFFER目标上。
         *  第二个参数指定传输数据的大小(以字节为单位)；用一个简单的sizeof计算出顶点数据大小就行。
         *  第三个参数是我们希望发送的实际数据。
         *  第四个参数指定了我们希望显卡如何管理给定的数据。它有三种形式：
                    GL_STATIC_DRAW ：数据不会或几乎不会改变。
                    GL_DYNAMIC_DRAW：数据会被改变很多。
                    GL_STREAM_DRAW ：数据每次绘制时都会改变。

                    三角形的位置数据不会改变，每次渲染调用时都保持原样，所以它的使用类型最好是GL_STATIC_DRAW。
                    如果，比如说一个缓冲中的数据将频繁被改变，那么使用的类型就是GL_DYNAMIC_DRAW或GL_STREAM_DRAW，
                    这样就能确保显卡把数据放在能够高速写入的内存部分。
         */
        glBufferData(GL_ARRAY_BUFFER, vtxStrides[0] * 3, vtxBuf[0], GL_STATIC_DRAW);

        // VBO 1 是 缓冲 颜色
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
        glBufferData(GL_ARRAY_BUFFER, vtxStrides[1] * 3, vtxBuf[1], GL_STATIC_DRAW);

        // VBO 2 实际上是 EBO， 缓冲 indices
        // 和顶点缓冲对象一样，EBO也是一个缓冲，它专门储存索引，OpenGL调用这些顶点的索引来决定该绘制哪个顶点。
        // 所谓的索引绘制(Indexed Drawing)正是我们问题的解决方案。

        // 与VBO类似，我们先绑定EBO然后用glBufferData把索引复制到缓冲里。
        // 同样，和VBO类似，我们会把这些函数调用放在绑定和解绑函数调用之间，只不过这次我们把缓冲的类型定义为GL_ELEMENT_ARRAY_BUFFER。
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 4, indices, GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    /**
     * glVertexAttribPointer函数的参数非常多，：
        第一个参数指定我们要配置的顶点属性。
            还记得我们在顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)吗？
            它可以把顶点属性的位置值设置为0。因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0。

        第二个参数指定顶点属性的大小。
            顶点属性是一个vec3，它由3个值组成，所以大小是3。

        第三个参数指定数据的类型。
            这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。

        第四个参数定义我们是否希望数据被标准化(Normalize)。
            如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。

        第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。
            要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）
            我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。
            一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，
            这个参数的意思简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节

        最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换。
            它表示位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0。我们会在后面详细解释这个参数。
     */
    //设置顶点属性指针
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT,
                          GL_FALSE, vtxStrides[0], nullptr);


    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT,
                          GL_FALSE, vtxStrides[1], nullptr);


    //我们传递了GL_ELEMENT_ARRAY_BUFFER当作缓冲目标。
    // 最后一件要做的事是用glDrawElements来替换glDrawArrays函数，来指明我们从索引缓冲渲染。
    // 使用glDrawElements时，我们会使用当前绑定的索引缓冲对象中的索引进行绘制
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[2]);
    /**
     * 第一个参数指定了我们绘制的模式，这个和glDrawArrays的一样。
     * 第二个参数是我们打算绘制顶点的个数，这里填3，也就是说我们一共需要绘制3个顶点。
     * 第三个参数是索引的类型，这里是GL_UNSIGNED_INT。
     * 最后一个参数里我们可以指定EBO中的偏移量（或者传递一个索引数组，但是这是当你不在使用索引缓冲对象的时候），但是我们会在这里填写0。
     *
     * glDrawElements函数从当前绑定到GL_ELEMENT_ARRAY_BUFFER目标的EBO中获取索引。
     * 这意味着我们必须在每次要用索引渲染一个物体时绑定相应的EBO，这还是有点麻烦。
     * 不过顶点数组对象同样可以保存索引缓冲对象的绑定状态。
     * VAO绑定时正在绑定的索引缓冲对象会被保存为VAO的元素缓冲对象。绑定VAO的同时也会自动绑定EBO。
     *
     * VAO 在下一个例子会讲解
     */
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void NativeTriangle3::shutdown() {
    // Delete program object
    GLUtils::DeleteProgram(mProgram);

    glDeleteBuffers(3, &vboIds[0]);
}
