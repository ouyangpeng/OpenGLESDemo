#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/swap

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;

float reflection = 0.4;
float perspective = 0.2;
float depth = 3.0;

const vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
const vec2 boundMin = vec2(0.0, 0.0);
const vec2 boundMax = vec2(1.0, 1.0);

bool inBounds (vec2 p) {
    return all(lessThan(boundMin, p)) && all(lessThan(p, boundMax));
}

vec2 project (vec2 p) {
    return p * vec2(1.0, -1.2) + vec2(0.0, -0.02);
}

vec4 bgColor (vec2 p, vec2 pfr, vec2 pto) {
    vec4 c = black;
    pfr = project(pfr);
    if (inBounds(pfr)) {
        c += mix(black, texture(u_texture0, pfr), reflection * mix(1.0, 0.0, pfr.y));
    }
    pto = project(pto);
    if (inBounds(pto)) {
        c += mix(black, texture(u_texture1, pto), reflection * mix(1.0, 0.0, pto.y));
    }
    return c;
}

vec4 transition(vec2 uv) {
    vec2 pfr, pto = vec2(-1.);

    float progress =  u_offset;

    float size = mix(1.0, depth, progress);
    float persp = perspective * progress;
    pfr = (uv + vec2(-0.0, -0.5)) * vec2(size/(1.0-perspective*progress), size/(1.0-size*persp*uv.x)) + vec2(0.0, 0.5);

    size = mix(1.0, depth, 1.- progress);
    persp = perspective * (1.- progress);
    pto = (uv + vec2(-1.0, -0.5)) * vec2(size/(1.0-perspective*(1.0-progress)), size/(1.0-size*persp*(0.5-uv.x))) + vec2(1.0, 0.5);

    if (progress < 0.5) {
        if (inBounds(pfr)) {
            return texture(u_texture0, pfr);
        }
        if (inBounds(pto)) {
            return texture(u_texture1, pto);
        }
    }
    if (inBounds(pto)) {
        return texture(u_texture1, pto);
    }
    if (inBounds(pfr)) {
        return texture(u_texture0, pfr);
    }
    return bgColor(uv, pfr, pto);
}

void main()
{
    outColor = transition(v_texCoord);
}