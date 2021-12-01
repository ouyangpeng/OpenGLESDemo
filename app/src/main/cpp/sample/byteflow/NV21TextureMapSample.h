//
// Created by OuyangPeng on 2021/12/1.
//

#ifndef OPENGLESDEMO_NV21TEXTUREMAPSAMPLE_H
#define OPENGLESDEMO_NV21TEXTUREMAPSAMPLE_H

#include <BaseGLSample.h>

#define NV21_TEXTURE_MAP_VERTEX_POS_INDEX              0
#define NV21_TEXTURE_MAP_VERTEX_TEXTCOORD_INDEX        1

#define NV21_TEXTURE_MAP_VERTEX_POS_SIZE               3
#define NV21_TEXTURE_MAP_VERTEX_TEXTCOORD_SIZE         2

class NV21TextureMapSample : public BaseGLSample {
public:
    NV21TextureMapSample();

    ~NV21TextureMapSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void create();

    virtual void draw();

    virtual void shutdown();

private:
    GLuint m_yTextureId;
    GLuint m_uvTextureId;

    GLint m_ySamplerLoc;
    GLint m_uvSamplerLoc;

    NativeImage mRenderImage;
};

// 渲染坐标系或OpenGLES 坐标系
// 4 个纹理坐标对于的顶点坐标分别为
// V0（-1，0.78），V1（-1，-0.78），V2（1，-0.78），V3（1，0.78）
// 可以参考图片： https://img-blog.csdnimg.cn/2019072219143123.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0tlbm5ldGhkcm9pZA==,size_16,color_FFFFFF,t_70#pic_center
static GLfloat yuv_verticesCoords[] = {
        -1.0f,  0.78f, 0.0f,  // Position 0
        -1.0f, -0.78f, 0.0f,  // Position 1
        1.0f,  -0.78f, 0.0f,  // Position 2
        1.0f,   0.78f, 0.0f,  // Position 3
};
// 纹理坐标系
// 4 个纹理坐标分别为
// T0（0，0），T1（0，1），T2（1，1），T3（1，0）。
// 可以参考图片： https://img-blog.csdnimg.cn/20190722191345500.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0tlbm5ldGhkcm9pZA==,size_16,color_FFFFFF,t_70#pic_center
static GLfloat yuv_textureCoords[] = {
        0.0f,  0.0f,        // TexCoord 0
        0.0f,  1.0f,        // TexCoord 1
        1.0f,  1.0f,        // TexCoord 2
        1.0f,  0.0f         // TexCoord 3
};

// 由于 OpenGLES 绘制是以三角形为单位的，设置绘制的 2 个三角形为 V0V1V2 和 V0V2V3 。
// 当我们调整纹理坐标的顺序顶点坐标顺序不变，如 T0T1T2T3 -> T1T2T3T0 ，绘制后将得到一个顺时针旋转 90 度的纹理贴图。
// 所以调整纹理坐标和顶点坐标的对应关系可以实现纹理图简单的旋转。

// 注意索引从0开始!
static GLushort yuv_indices[] = {
        0, 1, 2,        // 第一个三角形
        0, 2, 3         // 第二个三角形
};
#endif //OPENGLESDEMO_NV21TEXTUREMAPSAMPLE_H
