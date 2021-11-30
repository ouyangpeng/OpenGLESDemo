#include "NativeTriangle.h"

// 可以参考这篇讲解： https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
// 我们在OpenGL中指定的所有坐标都是3D坐标（x、y和z）
// 由于我们希望渲染一个三角形，我们一共要指定三个顶点，每个顶点都有一个3D位置。
// 我们会将它们以标准化设备坐标的形式（OpenGL的可见区域）定义为一个float数组。
// https://learnopengl-cn.github.io/img/01/04/ndc.png

// https://developer.android.com/guide/topics/graphics/opengl#kotlin
// 在 OpenGL 中，形状的面是由三维空间中的三个或更多点定义的表面。
// 一个包含三个或更多三维点（在 OpenGL 中被称为顶点）的集合具有一个正面和一个背面。
// 如何知道哪一面为正面，哪一面为背面呢？这个问题问得好！答案与环绕（即您定义形状的点的方向）有关。
// 查看图片 ： https://developer.android.com/images/opengl/ccw-winding.png
// 或者查看本地图片：Android_Java/Chapter_2/Hello_Triangle/ccw-winding.png
// 在此示例中，三角形的点按照使它们沿逆时针方向绘制的顺序定义。
// 这些坐标的绘制顺序定义了该形状的环绕方向。默认情况下，在 OpenGL 中，沿逆时针方向绘制的面为正面。
// 因此您看到的是该形状的正面（根据 OpenGL 解释），而另一面是背面。
//
// 知道形状的哪一面为正面为何如此重要呢？
// 答案与 OpenGL 的“面剔除”这一常用功能有关。
// 面剔除是 OpenGL 环境的一个选项，它允许渲染管道忽略（不计算或不绘制）形状的背面，从而节省时间和内存并缩短处理周期：
static GLfloat vVertices[] = {
        // 逆时针 三个顶点
        0.0f, 0.5f, 0.0f,            // 上角
        -0.5f, -0.5f, 0.0f,          // 左下角
        0.5f, -0.5f, 0.0f            // 右下角
};

void NativeTriangle::create() {
    GLUtils::printGLInfo();

    // Main Program
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_hello_triangle.glsl");
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle.glsl");

    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }
    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NativeTriangle::draw() {
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

    // Load the vertex data
    //  顶点着色器允许我们指定任何以顶点属性为形式的输入。这使其具有很强的灵活性的同时，
    //  它还的确意味着我们必须手动指定输入数据的哪一个部分对应顶点着色器的哪一个顶点属性。
    //  所以，我们必须在渲染前指定OpenGL该如何解释顶点数据。

    //  我们的顶点缓冲数据会被解析为下面这样子：https://learnopengl-cn.github.io/img/01/04/vertex_attribute_pointer.png
    //   . 位置数据被储存为32位（4字节）浮点值。
    //   . 每个位置包含3个这样的值。
    //   . 在这3个值之间没有空隙（或其他值）。这几个值在数组中紧密排列(Tightly Packed)。
    //   . 数据中第一个值在缓冲开始的位置。

    // 有了这些信息我们就可以使用glVertexAttribPointer函数告诉OpenGL该如何解析顶点数据（应用到逐个顶点属性上）了：
    // Load the vertex data

    // 第一个参数指定我们要配置的顶点属性。因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0。
    // 第二个参数指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3。
    // 第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
    // 第四个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
    // 第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。我们设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。
    //      一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，
    //      （译注: 这个参数的意思简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节）。
    // 最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在缓冲中起始位置的偏移量(Offset)。
    glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 0, vVertices);

    // 现在我们已经定义了OpenGL该如何解释顶点数据，
    // 我们现在应该使用glEnableVertexAttribArray，以顶点属性位置值作为参数，启用顶点属性；顶点属性默认是禁用的。
    glEnableVertexAttribArray(VERTEX_POS_INDX);

    // glDrawArrays函数第一个参数是我们打算绘制的OpenGL图元的类型。我们希望绘制的是一个三角形，这里传递GL_TRIANGLES给它。
    // 第二个参数指定了顶点数组的起始索引，我们这里填0。
    // 最后一个参数指定我们打算绘制多少个顶点，这里是3（我们只从我们的数据中渲染一个三角形，它只有3个顶点长）。
    //        public static final int GL_POINTS                                  = 0x0000;
    //        public static final int GL_LINES                                   = 0x0001;
    //        public static final int GL_LINE_LOOP                               = 0x0002;
    //        public static final int GL_LINE_STRIP                              = 0x0003;
    //        public static final int GL_TRIANGLES                               = 0x0004;
    //        public static final int GL_TRIANGLE_STRIP                          = 0x0005;
    //        public static final int GL_TRIANGLE_FAN                            = 0x0006;
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 禁用 通用顶点属性数组
    glDisableVertexAttribArray(0);
}

void NativeTriangle::shutdown() {
    // Delete program object
    GLUtils::DeleteProgram(mProgram);
}
