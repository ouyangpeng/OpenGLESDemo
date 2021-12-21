#version 300 es
precision mediump float;

in vec2 v_texCoord;

//分别对应 4 个绑定的纹理对象，将渲染结果保存到 4 个纹理中
layout(location = 0) out vec4 outColor0;
layout(location = 1) out vec4 outColor1;
layout(location = 2) out vec4 outColor2;
layout(location = 3) out vec4 outColor3;

uniform sampler2D s_Texture;
void main()
{
    vec4 outputColor = texture(s_Texture, v_texCoord);
    outColor0 = outputColor;
    outColor1 = vec4(outputColor.r, 0.0, 0.0, 1.0);
    outColor2 = vec4(0.0, outputColor.g, 0.0, 1.0);
    outColor3 = vec4(0.0, 0.0, outputColor.b, 1.0);
}