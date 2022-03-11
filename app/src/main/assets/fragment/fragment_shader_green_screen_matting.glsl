#version 300 es
// 参考 https://www.jianshu.com/p/159744875386

precision mediump float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

//声明采用器
uniform sampler2D s_TextureMap;

const float pixel = 30.0;

void main()
{
    vec4 tc = texture(s_TextureMap, v_texCoord);
    float r = tc.x * 255.0;
    float g = tc.y * 255.0;
    float b = tc.z * 255.0;

    // 这里简单的认定 g>140.0 && r<128.0 && b<128.0 时为绿色。
    // 当是绿色的时候，就将其颜色换成白色。同时alpha值设置为0.0
    if (g>140.0 && r<128.0 && b<128.0){
        tc.x =1.0;
        tc.y =1.0;
        tc.z =1.0;
        tc.w =0.0;
    } else {
        tc.w =1.0;
    }

    // 这里，我们检测被采样的纹理颜色的alpha值是否低于0.1的阈值，
    //如果是的话，则丢弃这个片段。片段着色器保证了它只会渲染不是（几乎）完全透明的片段。
    if (tc.a<0.1) discard;

    outColor = tc, 1.0;
}