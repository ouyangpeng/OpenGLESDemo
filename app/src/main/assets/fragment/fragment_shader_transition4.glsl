#version 300 es
precision mediump float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;

// Number of total bars/columns
const int bars = 20;

// Multiplier for speed ratio. 0 = no variation when going down, higher = some elements go much faster
const float amplitude = 2.0;

// Further variations in speed. 0 = no noise, 1 = super noisy (ignore frequency)
const float noise = 0.1;

// Speed variation horizontally. the bigger the value, the shorter the waves
const float frequency = 0.5;

// How much the bars seem to \run\ from the middle of the screen first (sticking to the sides). 0 = no drip, 1 = curved drip
const float dripScale = 0.5;


// The code proper --------

float rand(int num) {
    return fract(mod(float(num) * 67123.313, 12.0) * sin(float(num) * 10.3) * cos(float(num)));
}

float wave(int num) {
    float fn = float(num) * frequency * 0.1 * float(bars);
    return cos(fn * 0.5) * cos(fn * 0.13) * sin((fn+10.0) * 0.3) / 2.0 + 0.5;
}

float drip(int num) {
    return sin(float(num) / float(bars - 1) * 3.141592) * dripScale;
}

float pos(int num) {
    return (noise == 0.0 ? wave(num) : mix(wave(num), rand(num), noise)) + (dripScale == 0.0 ? 0.0 : drip(num));
}

vec4 getFromColor(vec2 uv) {
    return texture(u_texture0, uv);
}

vec4 getToColor(vec2 uv) {
    return texture(u_texture1, uv);
}

vec4 transition(vec2 uv) {
    int bar = int(uv.x * (float(bars)));
    float scale = 1.0 + pos(bar) * amplitude;
    float phase = u_offset * scale;
    float posY = uv.y / vec2(1.0).y;
    vec2 p;
    vec4 c;
    if (phase + posY < 1.0) {
        p = vec2(uv.x, uv.y + mix(0.0, vec2(1.0).y, phase)) / vec2(1.0).xy;
        c = getFromColor(p);
    } else {
        p = uv.xy / vec2(1.0).xy;
        c = getToColor(p);
    }

    // Finally, apply the color
    return c;
}

void main()
{
    outColor = transition(v_texCoord);
}