#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

uniform sampler2D s_TextureMap;

in vec2 v_texCoord;

// 左眼中心点
uniform highp vec2 u_LeftEyeCenterPos;
// 右眼中心点
uniform highp vec2 u_RightEyeCenterPos;
// 放大系数
uniform highp float u_ScaleRatio;
// 影响半径
uniform highp float u_Radius;
//图片分辨率
uniform vec2 u_ImgSize;

// centerPos 眼睛点
// curPos 当前要采样的点
// radius 最大作用半径
vec2 warpEyes(vec2 centerPos, vec2 curPos, float rmax, float scaleRatio)
{
    vec2 result = curPos;
    vec2 imgCurPos = curPos * u_ImgSize;
    // 获取当前画的点和眼睛的距离
    float r = distance(imgCurPos, centerPos);
    // 在radius范围内 才放大
    if (r < rmax)
    {
        // 公式：fsr = ( 1 -  pow(r/rmax -1 , 2 ) * scaleRatio ) * r
        // fsr是个float，是距离eye的距离，我们需要得到点的位置是一个向量
        // 那么根据比例关系可以得出：
        // 新点与eye的向量差 /  老点与eye的向量差 = 新点与eye的距离 /  老点与eye的距离
        // (result - centerPos) / ( imgCurPos - centerPos) = fsr / r
        // result - centerPos = fsr / r * ( imgCurPos - centerPos)
        // result = centerPos + fsr / r * ( imgCurPos - centerPos)
        float fsr = 1.0 - scaleRatio * pow(smoothstep(0.0, 1.0, r / rmax) - 1.0, 2.0);
        result = centerPos + fsr * (imgCurPos - centerPos);
        result = result / u_ImgSize;
    }
    return result;
}

void main()
{
    // 连续调用两次warpEyes方法，分别放大左眼和右眼
    // 放大左眼
    vec2 newTexCoord = warpEyes(u_LeftEyeCenterPos, v_texCoord, u_Radius, u_ScaleRatio);
    // 放大右眼
    newTexCoord = warpEyes(u_RightEyeCenterPos, newTexCoord, u_Radius, u_ScaleRatio);
    outColor = texture(s_TextureMap, newTexCoord);
}
