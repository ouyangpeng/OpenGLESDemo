#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/cube

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;


const float persp  = 0.7;
const float unzoom = 0.3;
const float reflection = 0.4;
const float floating = 3.0;

vec2 project (vec2 p) {
    return p * vec2(1.0, -1.2) + vec2(0.0, -floating/100.);
}

bool inBounds (vec2 p) {
    return all(lessThan(vec2(0.0), p)) && all(lessThan(p, vec2(1.0)));
}

vec4 bgColor (vec2 p, vec2 pfr, vec2 pto) {
    vec4 c = vec4(0.0, 0.0, 0.0, 1.0);
    pfr = project(pfr);
    if (inBounds(pfr)) {
        c += mix(vec4(0.0), texture(u_texture0,pfr), reflection * mix(1.0, 0.0, pfr.y));
    }
    pto = project(pto);
    if (inBounds(pto)) {
        c += mix(vec4(0.0), texture(u_texture1,pto), reflection * mix(1.0, 0.0, pto.y));
    }
    return c;
}

// p : the position
// persp : the perspective in [ 0, 1 ]
// center : the xcenter in [0, 1] \ 0.5 excluded
vec2 xskew (vec2 p, float persp, float center) {
    float x = mix(p.x, 1.0-p.x, center);
    return (
        (vec2(x, (p.y - 0.5*(1.0-persp) * x) / (1.0+(persp-1.0)*x))- vec2(0.5-distance(center, 0.5), 0.0))
        * vec2(0.5 / distance(center, 0.5) * (center<0.5 ? 1.0 : -1.0), 1.0)
        + vec2(center<0.5 ? 0.0 : 1.0, 0.0));
}

vec4 transition(vec2 op) {
    float uz = unzoom * 2.0*(0.5-distance(0.5, u_offset));
    vec2 p = -uz*0.5+(1.0+uz) * op;
    vec2 fromP = xskew(
        (p - vec2(u_offset, 0.0)) / vec2(1.0-u_offset, 1.0),
        1.0-mix(u_offset, 0.0, persp),
        0.0
    );
    vec2 toP = xskew(
        p / vec2(u_offset, 1.0),
        mix(pow(u_offset, 2.0), 1.0, persp),
        1.0
    );
    if (inBounds(fromP)) {
        return texture(u_texture0,fromP);
    }
    else if (inBounds(toP)) {
        return texture(u_texture1,toP);
    }
    return bgColor(op, fromP, toP);
}



void main()
{
    outColor = transition(v_texCoord);
}