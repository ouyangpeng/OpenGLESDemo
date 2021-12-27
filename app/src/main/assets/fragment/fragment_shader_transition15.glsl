#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/GridFlip

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;



const ivec2 size = ivec2(4);
const float pause = 0.1;
const float dividerWidth = 0.05;
const vec4 bgcolor = vec4(0.0, 0.0, 0.0, 1.0);
const float randomness = 0.1;

float rand (vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float getDelta(vec2 uv) {
    vec2 rectanglePos = floor(vec2(size) * uv);
    vec2 rectangleSize = vec2(1.0 / vec2(size).x, 1.0 / vec2(size).y);
    float top = rectangleSize.y * (rectanglePos.y + 1.0);
    float bottom = rectangleSize.y * rectanglePos.y;
    float left = rectangleSize.x * rectanglePos.x;
    float right = rectangleSize.x * (rectanglePos.x + 1.0);
    float minX = min(abs(uv.x - left), abs(uv.x - right));
    float minY = min(abs(uv.y - top), abs(uv.y - bottom));
    return min(minX, minY);
}

float getDividerSize() {
    vec2 rectangleSize = vec2(1.0 / vec2(size).x, 1.0 / vec2(size).y);
    return min(rectangleSize.x, rectangleSize.y) * dividerWidth;
}

vec4 transition(vec2 uv) {
    if(u_offset < pause) {
        float currentProg = u_offset / pause;
        float a = 1.0;
        if(getDelta(uv) < getDividerSize()) {
            a = 1.0 - currentProg;
        }
        return mix(bgcolor, texture(u_texture0,uv), a);
    }
    else if(u_offset < 1.0 - pause){
        if(getDelta(uv) < getDividerSize()) {
            return bgcolor;
        } else {
            float currentProg = (u_offset - pause) / (1.0 - pause * 2.0);
            vec2 q = uv;
            vec2 rectanglePos = floor(vec2(size) * q);

            float r = rand(rectanglePos) - randomness;
            float cp = smoothstep(0.0, 1.0 - r, currentProg);

            float rectangleSize = 1.0 / vec2(size).x;
            float delta = rectanglePos.x * rectangleSize;
            float offset = rectangleSize / 2.0 + delta;

            uv.x = (uv.x - offset)/abs(cp - 0.5)*0.5 + offset;
            vec4 a = texture(u_texture0,uv);
            vec4 b = texture(u_texture1,uv);

            float s = step(abs(vec2(size).x * (q.x - delta) - 0.5), abs(cp - 0.5));
            return mix(bgcolor, mix(b, a, step(cp, 0.5)), s);
        }
    }
    else {
        float currentProg = (u_offset - 1.0 + pause) / pause;
        float a = 1.0;
        if(getDelta(uv) < getDividerSize()) {
            a = currentProg;
        }
        return mix(bgcolor, texture(u_texture1,uv), a);
    }
}



void main()
{
    outColor = transition(v_texCoord);
}