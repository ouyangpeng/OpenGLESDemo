#version 300 es                                    
precision mediump float;
in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

// y_texture 和 uv_texture 分别是 NV21 Y Plane 和 UV Plane 纹理的采样器，
// 对两个纹理采样之后组成一个（y,u,v）三维向量，之后左乘变换矩阵转换为（r,g,b）三维向量。
uniform sampler2D y_texture;
uniform sampler2D uv_texture;
void main()
{
    vec3 yuv;
    // YUV是一种颜色空间，基于YUV的颜色编码是流媒体的常用编码方式。
    // Y表示流明，U、V表示色度、浓度，这种表达方式起初是为了彩色电视与黑白电视之间的信号兼容。对于图像每一点，Y确定其亮度，UV确认其彩度。
    // Y’CbCr也称为YUV，是YUV的压缩版本，不同之处在于Y’CbCr用于数字图像领域，YUV用于模拟信号领域，
    // MPEG、DVD、摄像机中常说的YUV其实是Y’CbCr，二者转换为RGBA的转换矩阵是不同的。Y’为亮度，Cb、Cr分量代表当前颜色对蓝色和红色的偏移程度。

    // YUV 图不能直接用于显示，需要转换为 RGB 格式，而 YUV 转 RGB 是一个逐像素处理的耗时操作，
    // 在 CPU 端进行转换效率过低，这时正好可以利用 GPU 强大的并行处理能力来实现 YUV 到 RGB 的转换。

    // 只赋值x 则会出现黑白照片效果
    yuv.x = texture(y_texture, v_texCoord).r;

    // 因为YUV转RGB用的是Y、U-128和V-128，texture函数返回向量值的范围是0-1.0，128表示0.5, 所以要减去0.5
    yuv.y = texture(uv_texture, v_texCoord).a-0.5;
    yuv.z = texture(uv_texture, v_texCoord).r-0.5;

    //      RGB转YUV:
    //      Y = 0.299 * R + 0.587 * G + 0.114*B
    //      U = - 0.1687 * R - 0.3313 * G + 0.5* B + 128    注： +128 的含义是让UV的范围处于整数区间（0-255）
    //      V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128     注： +128 的含义是让UV的范围处于整数区间（0-255）

    //      YUV转RGB
    //      R = Y + 1.402 * (V - 128)
    //      G = Y - 0.34414 * (U - 128) - 0.71414 * (V - 128)
    //      B = Y + 1.772 * (U - 128)

    //  YUV 和 RGB 的转换矩阵
    highp vec3 rgb =  mat3( 1.0, 1.0, 1.0,       //第一列
                            0.0, -0.34414, 1.772,   //第二列
                            1.403, -0.71414, 0.0    //第三列
                        ) * yuv;
    outColor = vec4(rgb, 1);
}



