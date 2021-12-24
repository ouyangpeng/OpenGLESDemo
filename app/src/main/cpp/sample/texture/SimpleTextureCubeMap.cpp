//
// Created by OuyangPeng on 2021/10/24.
//

// 可以参考链接：
// https://www.cnblogs.com/alps/p/7112872.html
// https://blog.csdn.net/huangzhipeng/article/details/7957233

#include <cstdlib>
#include "SimpleTextureCubeMap.h"

void SimpleTextureCubeMap::Create() {
    GLUtils::printGLInfo();

    // Main Program
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_simple_texture_cubemap.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_simple_texture_cubemap.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }
    // Get the sampler location
    samplerLoc = glGetUniformLocation(m_ProgramObj, "s_texture");

    // Load the texture  加载纹理
    textureId = CreateSimpleTextureCubemap();

    numIndices = esGenSphere(20, 0.75f, &vertices, &normals,
                             nullptr, &indices);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}


void SimpleTextureCubeMap::Draw() {

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // Use the program object
    glUseProgram(m_ProgramObj);

    // Load the vertext position
    glVertexAttribPointer(SIMPLE_TEXTURE_CUBE_MAP_VERTEX_POS_INDEX, 3, GL_FLOAT,
                          GL_FALSE, 0, vertices);

    // Load the normal
    glVertexAttribPointer(SIMPLE_TEXTURE_CUBE_MAP_VERTEX_NORMAL_INDEX, 3, GL_FLOAT,
                          GL_FALSE, 0, normals);

    glEnableVertexAttribArray(SIMPLE_TEXTURE_CUBE_MAP_VERTEX_POS_INDEX);
    glEnableVertexAttribArray(SIMPLE_TEXTURE_CUBE_MAP_VERTEX_NORMAL_INDEX);

    // Bind the texture
    // 激活当前纹理单元，以便后续的glBindTexture调用将纹理绑定到当前活动单元
    glActiveTexture(GL_TEXTURE0);
    // 纹理绑定到纹理单元0
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    // Set the sampler texture unit to 0
    // 采样器设置为纹理单元0
    glUniform1i(samplerLoc, 0);

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);
}

// Create a simple cubemap with a 1x1 face with a different color for each face
GLuint SimpleTextureCubeMap::CreateSimpleTextureCubemap() {
    // 基本上说cubemap它包含6个2D纹理，这每个2D纹理是一个立方体（cube）的一个面，也就是说它是一个有贴图的立方体。

    // Six 1x1 RGB faces
    GLubyte cubePixels[6][3] =
            {
                    // Face 0 - Red
                    255, 0, 0,
                    // Face 1 - Green,
                    0, 255, 0,
                    // Face 2 - Blue
                    0, 0, 255,
                    // Face 3 - Yellow
                    255, 255, 0,
                    // Face 4 - Purple
                    255, 0, 255,
                    // Face 5 - White
                    255, 255, 255
            };

    // 创建一个Cubemap
    // Cubemap和其他纹理一样，所以要创建一个cubemap，在进行任何纹理操作之前，需要生成一个纹理，
    // 激活相应纹理单元然后绑定到合适的纹理目标上。这次要绑定到 GL_TEXTURE_CUBE_MAP纹理类型
    GLuint textureId;
    // Generate a texture object
    glGenTextures(1, &textureId);
    // Bind the texture object
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);



    // 由于cubemap包含6个纹理，立方体的每个面一个纹理，我们必须调用glTexImage2D函数6次，函数的参数和前面教程讲的相似。
    // 然而这次我们必须把纹理目标（target）参数设置为cubemap特定的面，这是告诉OpenGL我们创建的纹理是对应立方体哪个面的。
    // 因此我们便需要为cubemap的每个面调用一次 glTexImage2D。

    // 由于cubemap有6个面，OpenGL就提供了6个不同的纹理目标，来应对cubemap的各个面。

    // 纹理目标（Texture target）	方位
    //  GL_TEXTURE_CUBE_MAP_POSITIVE_X	右
    //  GL_TEXTURE_CUBE_MAP_NEGATIVE_X	左
    //  GL_TEXTURE_CUBE_MAP_POSITIVE_Y	上
    //  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	下
    //  GL_TEXTURE_CUBE_MAP_POSITIVE_Z	后
    //  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	前

    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
    //#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
    //#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A

    //  指定6个面的纹理图像

    // 使用循环 替换下面的6个方法
    // 很多OpenGL其他枚举一样，对应的int值都是连续增加的，所以我们有一个纹理位置的数组或vector，
    // 就能以GL_TEXTURE_CUBE_MAP_POSITIVE_X为起始来对它们进行遍历，每次迭代枚举值加 1，这样循环所有的纹理目标效率较高：
    for (GLuint i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1, 1, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[i]);
    }

// Load the cube face - Positive X
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 1, 1, 0,
//                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[0]);

//    // Load the cube face - Negative X
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 1, 1, 0,
//                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[1]);
//
//    // Load the cube face - Positive Y
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 1, 1, 0,
//                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[2]);
//
//    // Load the cube face - Negative Y
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 1, 1, 0,
//                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[3]);
//
//    // Load the cube face - Positive Z
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 1, 1, 0,
//                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[4]);
//
//    // Load the cube face - Negative Z
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 1, 1, 0,
//                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[5]);


    // 由于cubemap和其他纹理没什么不同，我们也要定义它的环绕方式和过滤方式：
    // Set the filtering mode
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureId;
}

void SimpleTextureCubeMap::Shutdown() {
    if (vertices != nullptr) {
        free(vertices);
    }

    if (indices != nullptr) {
        free(indices);
    }

    if (normals != nullptr) {
        free(normals);
    }

    // Delete texture object
    glDeleteTextures(1, &textureId);

    // Delete program object
    GLUtils::DeleteProgram(m_ProgramObj);
}