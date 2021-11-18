#version 300 es                                   
precision mediump float;
// 噪音畸变的雾化片段着色器

uniform mediump sampler3D s_noiseTex;
uniform float u_fogMaxDist;
uniform float u_fogMinDist;
uniform vec4  u_fogColor;
uniform float u_time;

in vec4 v_color;
in vec2 v_texCoord;
in vec4 v_eyePos;

layout(location = 0) out vec4 outColor;

float computeLinearFogFactor()
{
    float factor;
    // Compute linear fog equation
    float dist = distance(v_eyePos, vec4(0.0, 0.0, 0.0, 1.0));
    factor = (u_fogMaxDist - dist) / (u_fogMaxDist - u_fogMinDist);
    // Clamp in the [0,1] range
    factor = clamp(factor, 0.0, 1.0);
    return factor;
}

void main(void)
{
    float fogFactor = computeLinearFogFactor();
    vec3 noiseCoord =  vec3(v_texCoord.xy - u_time, u_time);
    fogFactor -= texture(s_noiseTex, noiseCoord).r * 0.25;
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 baseColor = v_color;
    outColor = baseColor * fogFactor + u_fogColor * (1.0 - fogFactor);
}