#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/doorway

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;



const float reflection  = 0.4;
const float perspective  = 0.4;
const float depth  = 3.0;

const vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
const vec2 boundMin = vec2(0.0, 0.0);
const vec2 boundMax = vec2(1.0, 1.0);

bool inBounds (vec2 p) {
    return all(lessThan(boundMin, p)) && all(lessThan(p, boundMax));
}

vec2 project (vec2 p) {
    return p * vec2(1.0, -1.2) + vec2(0.0, -0.02);
}

vec4 bgColor (vec2 p, vec2 pto) {
    vec4 c = black;
    pto = project(pto);
    if (inBounds(pto)) {
        c += mix(black, texture(u_texture1,pto), reflection * mix(1.0, 0.0, pto.y));
    }
    return c;
}


vec4 transition (vec2 p) {
    vec2 pfr = vec2(-1.), pto = vec2(-1.);
    float middleSlit = 2.0 * abs(p.x-0.5) - u_offset;
    if (middleSlit > 0.0) {
        pfr = p + (p.x > 0.5 ? -1.0 : 1.0) * vec2(0.5*u_offset, 0.0);
        float d = 1.0/(1.0+perspective*u_offset*(1.0-middleSlit));
        pfr.y -= d/2.;
        pfr.y *= d;
        pfr.y += d/2.;
    }
    float size = mix(1.0, depth, 1.-u_offset);
    pto = (p + vec2(-0.5, -0.5)) * vec2(size, size) + vec2(0.5, 0.5);
    if (inBounds(pfr)) {
        return texture(u_texture0,pfr);
    }
    else if (inBounds(pto)) {
        return texture(u_texture1,pto);
    }
    else {
        return bgColor(p, pto);
    }
}

void main()
{
    outColor = transition(v_texCoord);
}