#version 300 es
precision mediump float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_Texture0;
uniform sampler2D s_Texture1;
uniform sampler2D s_Texture2;
uniform sampler2D s_Texture3;

// 用于渲染（采样） 4 个纹理的片段着色器，实际上是将 4 张图做一个拼接。
void main()
{
    if (v_texCoord.x < 0.5 && v_texCoord.y < 0.5)
    {
        outColor = texture(s_Texture0, v_texCoord);
    }
    else if (v_texCoord.x > 0.5 && v_texCoord.y < 0.5)
    {
        outColor = texture(s_Texture1, v_texCoord);
    }
    else if (v_texCoord.x < 0.5 && v_texCoord.y > 0.5)
    {
        outColor = texture(s_Texture2, v_texCoord);
    }
    else
    {
        outColor = texture(s_Texture3, v_texCoord);
    }
}