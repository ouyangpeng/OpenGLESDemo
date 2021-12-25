#version 300 es
precision mediump float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D u_texture0;//rgba
uniform sampler2D u_texture1;//gray  人像灰度图
uniform sampler2D u_texture2;//mapping

uniform float u_offset;
uniform vec2 u_texSize;
uniform float u_time;

void main()
{
    // 首先对 mask 进行采样
    float gray = texture(u_texture1, v_texCoord).r;
    vec4 rgba  = texture(u_texture0, v_texCoord);

    // 然后判断采样点是否位于人像区域（灰度值是否大于 0.01 ），
    // 若采样点位于人像区域外，对颜色进行灰度化。
    if (gray > 0.01) {
        outColor = rgba;
    }
    else
    {
        vec2 fragCoord = gl_FragCoord.xy;
        vec2 p = (- u_texSize.xy + 2.0 * fragCoord)/u_texSize.y;
        float a = atan(p.y, p.x);
        float r = pow(pow(p.x * p.x, 4.0) + pow(p.y * p.y, 4.0), 1.0 / 8.0);
        vec2 uv = vec2(1.0 / r + 0.2 * u_time, a);
        float f = cos(12.0 * uv.x) * cos(6.0 * uv.y);
        vec3 col = 0.5 + 0.5 * sin(3.1416 * f + vec3(0.0, 0.5, 1.0));
        col = col * r;
        vec4 tunnelColor = vec4(col, 1.0);

        //RGB 灰度化
        float Y = 0.299 * rgba.r + 0.587 * rgba.g + 0.114 * rgba.b;
        vec4 grayColor = vec4(vec3(Y), 1.0);
        //混合渐变
        outColor = mix(grayColor, tunnelColor, u_offset);
    }
}