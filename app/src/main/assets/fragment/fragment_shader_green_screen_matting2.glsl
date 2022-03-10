#version 300 es
// 参考 https://blog.csdn.net/u010302327/article/details/109842237
// https://gitcode.net/mirrors/a422070876/TestGreenScreen/-/blob/master/app/src/main/java/com/hyq/hm/test/greenscreen/GLDrawRenderer.java
precision mediump float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

//声明采用器
uniform sampler2D s_TextureMap;

// 可根据自己的需求进行调节，太小了会扣太多，太大了会扣太少，我这里设0.05正好
float smoothValue = 0.05f;


// 先把rgb转成hsv，通过rgb的g和hsv的颜色范围来确定绿色，再使用算法
highp vec3 rgb2hsv(highp vec3 c){
    highp vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    highp vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    highp vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    highp float d = q.x - min(q.w, q.y);
    highp float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main() {
    highp vec4 rgba = texture(s_TextureMap, v_texCoord);
    highp float rbAverage = (rgba.r + rgba.b)*0.8;
    // 先把rgb转成hsv，通过rgb的g和hsv的颜色范围来确定绿色，再使用算法
    highp vec3 hsv = rgb2hsv(rgba.rgb);
    highp float hmin = 0.19444000;
    highp float hmax = 0.42777888;
    highp float smin = 0.16862000;
    highp float smax = 1.0;
    highp float vmin = 0.18039000;
    highp float vmax = 1.0;
    int gs = 0;
    if (hsv.x >= hmin && hsv.x <= hmax &&
    hsv.y >= smin && hsv.y <= smax &&
    hsv.z >= vmin && hsv.z <= vmax){
        gs = 1;
    } else if (rgba.g >= rbAverage && rgba.g > 0.6){
        gs = 1;
    }
    if (gs == 1){
        rbAverage = (rgba.r + rgba.b)*0.65;
        if (rbAverage > rgba.g)rbAverage = rgba.g;
        highp float gDelta = rgba.g - rbAverage;
        highp float ss = smoothstep(0.0, smoothValue, gDelta);
        rgba.a = 1.0 - ss;
        rgba.a = rgba.a * rgba.a * rgba.a;
        rgba = mix(vec4(0.0), rgba, rgba.a);
    }

    // 这里，我们检测被采样的纹理颜色的alpha值是否低于0.1的阈值，
    //如果是的话，则丢弃这个片段。片段着色器保证了它只会渲染不是（几乎）完全透明的片段。
    if (rgba.a<0.1) discard;

    outColor = rgba;
}
