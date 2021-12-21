#version 300 es
precision highp float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

//采样器
uniform sampler2D s_TextureMap;
//点击的位置（归一化）
uniform vec2 u_TouchXY;
//纹理尺寸
uniform vec2 u_TexSize;
//归一化的时间
uniform float u_Time;
//边界 0.1
uniform float u_Boundary;

void main()
{
    float ratio = u_TexSize.y / u_TexSize.x;
    vec2 texCoord = v_texCoord * vec2(1.0, ratio);
    vec2 touchXY = u_TouchXY * vec2(1.0, ratio);
    float distance = distance(texCoord, touchXY);
    if ((u_Time - u_Boundary) > 0.0
    && (distance <= (u_Time + u_Boundary))
    && (distance >= (u_Time - u_Boundary))) {
        //输入 diff = x
        float diff = (distance - u_Time);

        //采样坐标移动距离

        // 波动效果1
        //平滑函数 y=20.0 * x * (x - 0.1)*(x + 0.1)
        // float moveDis = 20.0*diff*(diff - u_Boundary)*(diff + u_Boundary);

        // 波动效果2
        // 古怪的函数，y= (1-Math.pow(Math.abs(20*x), 4.8))*x
        float moveDis = 1.0 - pow(abs(20.0 * diff), 4.8) * diff;

        //单位方向向量
        vec2 unitDirectionVec = normalize(texCoord - touchXY);
        //采样坐标偏移（实现放大和缩小效果）
        texCoord = texCoord + (unitDirectionVec * moveDis);
    }
    //转换回来
    texCoord = texCoord / vec2(1.0, ratio);
    outColor = texture(s_TextureMap, texCoord);
}