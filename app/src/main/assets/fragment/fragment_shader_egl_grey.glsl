#version 300 es
precision highp float;

// 灰度图

//灰度滤镜原理：
//浮点算法：Gray = R * 0.3 + G * 0.59 + B * 0.11
//整数算法：Gray = (R30 + G59 + B*11) / 100
//移位算法：Gray = (R76 + G151 + B*28)>>8
//平均值法：Gray = (R+G+B)/3
//仅取绿色：Gray = G


layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

// 方法1 权值法 （各个颜色的权值参考了GPUImage ，也可以用上面的权值）
vec4 grey1(){
    // 权重值取自GPUImage，RGB三值相加=1
    const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

    // 获取纹理图片当前纹理坐标下的 颜色值
    lowp vec4 textureColor = texture(s_TextureMap, v_texCoord);

    // 点乘 : 颜色 点乘 变换因子
    float luminance = dot(textureColor.rgb, W);

    // 将灰度值转化并填充到像素中(luminance，luminance，luminance，textureColor.a)
    return vec4(vec3(luminance), textureColor.a);
}

// 方法2 平均值法
vec4 grey2(){
    // 方法2
    lowp vec4 textureColor = texture(s_TextureMap, v_texCoord);
    float color = (textureColor.r + textureColor.g + textureColor.b) / 3.0;
    vec4 tempColor = vec4(color, color, color, 1);
    return vec4(vec3(tempColor), 1.0);
}

void main() {
        outColor = grey1();
//    outColor = grey2();
}