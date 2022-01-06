#version 300 es
precision highp float;


// 浮雕效果是指图像的前景前向凸出背景。
// 实现思路：把图象的一个象素和左上方的象素进行求差运算，并加上一个灰度。
// 这个灰度就是表示背景颜色。这里我们设置这个插值为128 (图象RGB的值是0-255)。
// 同时,我们还应该把这两个颜色的差值转换为亮度信息，避免浮雕图像出现彩色像素。

// https://www.jianshu.com/p/e4a8c83cd373
// http://www.cppblog.com/biao/archive/2009/05/30/86171.html

layout(location = 0) out vec4 outColor;
// 纹理坐标
in vec2 v_texCoord;
// 纹理采样器
uniform sampler2D s_TextureMap;

// 权重值取自GPUImage，RGB三值相加=1
const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

const vec2 TexSize = vec2(100.0, 100.0);
// 灰度
const vec4 bkColor = vec4(0.5, 0.5, 0.5, 1.0);

void main() {
    // 获取纹理图片当前纹理坐标下的 颜色值
    vec4 curColor = texture(s_TextureMap, v_texCoord);

    vec2 upLeftUV = vec2 (
        v_texCoord.x - 1.0 / TexSize.x,
        v_texCoord.y - 1.0 / TexSize.y
    );
    // 获取纹理图片当前纹理坐标的左上方的像素的 颜色值
    vec4 upLeftColor = texture(s_TextureMap, upLeftUV);

    // 把图象的一个象素和左上方的象素进行求差运算
    vec4 delColor = curColor - upLeftColor;

    // 点乘 : 颜色 点乘 变换因子
    float luminance = dot(delColor.rgb, W);

    // 将灰度值转化并填充到像素中(luminance，luminance，luminance，0.0)
    outColor = vec4(vec3(luminance), 0.0) + bkColor;
}