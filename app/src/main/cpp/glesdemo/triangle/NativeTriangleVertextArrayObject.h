#pragma once

#include "../../utils/GLUtils.h"

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_STRIDE         ( sizeof(GLfloat) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ) )

class NativeTriangleVAO : public BaseGLSample {
public:
    NativeTriangleVAO();

    virtual ~NativeTriangleVAO();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();

private:
    // VertexBufferObject Ids
    GLuint vboIds[2];

    // VertexArrayObject Id
    /**
     * 顶点数组对象(Vertex Array Object, VAO)可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。
     * 这样的好处就是，当配置顶点属性指针时，你只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了。
     * 这使在不同顶点数据和属性配置之间切换变得非常简单，只需要绑定不同的VAO就行了。刚刚设置的所有状态都将存储在VAO中
     *
     * OpenGL的核心模式要求我们使用VAO，所以它知道该如何处理我们的顶点输入。如果我们绑定VAO失败，OpenGL会拒绝绘制任何东西。
     * 一个顶点数组对象会储存以下这些内容：
            glEnableVertexAttribArray和glDisableVertexAttribArray的调用。
            通过glVertexAttribPointer设置的顶点属性配置。
            通过glVertexAttribPointer调用与顶点属性关联的顶点缓冲对象。
     */
    GLuint vaoId;

};

