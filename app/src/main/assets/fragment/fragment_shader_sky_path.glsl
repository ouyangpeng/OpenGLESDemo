#version 300 es
precision highp float;

/*
 * Original shader from: https://www.shadertoy.com/view/3sBGzV
 * https://glslsandbox.com/e#77894.0
 */

layout(location = 0) out vec4 outColor;

// glslsandbox uniforms
uniform float u_time;
uniform vec2 u_screenSize;

// --------[ Original ShaderToy begins here ]---------- //

#define MARCH_STEPS 100

float PI = acos(-1.0);

float box(vec3 p, vec3 s) {
    vec3 ap=abs(p)-s;
    return length(max(vec3(0), ap)) + min(0.0, max(ap.x, max(ap.y, ap.z)));
}

mat2 rot(float a) {
    float ca=cos(a);
    float sa=sin(a);
    return mat2(ca, sa, -sa, ca);
}

vec3 tunnel(vec3 p) {
    vec3 off=vec3(0);
    off.x += sin(p.z*0.6)*0.7;
    off.x += sin(p.z*0.17)*1.5;

    off.y += cos(p.z*0.27)*0.5;
    off.y += cos(p.z*0.34)*0.25;
    return off;
}

float stair(vec3 p, float s1, float s2) {

    p.z = (fract(p.z/s1-0.5)-0.5)*s1;
    p.yz *= rot(PI*0.25);
    float b = box(p, vec3(s2, s1, s1));

    return b;
}

float map(vec3 p) {

    float rep = 10.0;
    p.y -= max(0.0, abs(p.x)-10.0)*0.3;
    p.x = (fract(p.x/rep-0.5)-0.5)*rep;

    p += tunnel(p);

    vec3 rp = p;
    float boxrep = 10.0;
    rp.z = (fract(rp.z/boxrep-0.5)-0.5)*boxrep;

    vec3 rp2 = p;
    float boxrep2 = 1.0;
    rp2.x=abs(rp2.x)-0.4;
    rp2.z = (fract(rp2.z/boxrep2-0.5)-0.5)*boxrep2;

    float b = box(rp + vec3(0, -9, 0), vec3(0.6, 10.5, 0.6));
    vec3 rp3 = rp + vec3(0, 1.5, 0);
    rp3.xy *= rot(PI*0.3);
    rp3.yz *= rot(PI*0.3);
    float b2 = box(rp3, vec3(0.7));
    b2 = max(b2, p.y+1.5);
    b = min(b, b2);



    float st = stair(p, 0.1, 0.4);
    float st2 = stair(p + vec3(0, 0.7, 0), 0.6, 0.4);

    b = max(b, -st2);

    float c = box(rp2 + vec3(0, 0.3, 0), vec3(0.05, 0.3, 0.2));
    rp2.y = abs(rp2.y + 0.43)-0.1;
    c = min(c, box(rp2, vec3(0.03, 0.03, 1.0)));



    return min(c, min(b, st));
}


vec3 norm(vec3 p) {
    vec2 off=vec2(0.01, 0);
    return normalize(map(p)-vec3(map(p-off.xyy), map(p-off.yxy), map(p-off.yyx)));
}

float rnd(vec2 uv) {
    return fract(dot(sin(uv*723.588+uv.yx*413.877), vec2(9865.535)));
}

float rnd(float t) {
    return fract(sin(t*435.232)*7412.223);
}

float curve(float t, float d) {
    float g=t/d;
    return mix(rnd(floor(g)), rnd(floor(g)+1.0), pow(smoothstep(0.0, 1.0, fract(g)), 10.0));
}

float curve2(float t, float d) {
    float g=t/d;
    float ig = floor(g);
    float fg = fract(g);
    fg = smoothstep(0.0, 1.0, fg);
    fg = pow(fg, rnd(ig)*2.0+0.5);
    //fg = smoothstep(0,1,fg);
    return (ig+fg)*d;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = vec2(fragCoord.x / u_screenSize.x, fragCoord.y / u_screenSize.y);
    uv -= 0.5;
    uv /= vec2(u_screenSize.y / u_screenSize.x, 1);

    float rand=rnd(uv);
    float dither = 0.8+0.1*rand;

    vec3 s=vec3(0, -1, 0);
    vec3 t=vec3(0, -0.3, 3);
    vec3 tar=vec3(0, -0.7, 3);

    //dashxdr was here got rid of the ****ing surging
    float motion = curve2(u_time*1.5, 0.8)*3.0;
    s.z += motion;
    t.z += motion;
    //tar.z += u_time*1.5*3.0;// + pow(curve(u_time, 0.7),2)*5.0;
    float offset = 15.0;
    tar.z += (curve2(u_time*1.5-offset, 1.1)+offset)*3.0 + pow(curve(u_time, 0.9), 3.0)*5.0;

    s-=tunnel(s);
    vec3 offt = tunnel(t);
    t-=offt;
    tar-=tunnel(tar);

    vec3 cz=normalize(t-s);
    vec3 cx=normalize(cross(cz, vec3(0, 1, 0) - offt*0.2));
    vec3 cy=normalize(cross(cz, cx));

    vec3 r = normalize(cx*uv.x + cy*uv.y + cz*(0.7+sin(u_time*2.0)*0.4));

    vec3 col = vec3(0);
    vec3 l = normalize(vec3(-0.7, -1.0, -0.5));

    vec3 back = mix(vec3(0.1, 0.3, 1.0)*0.1, vec3(1.0, 0.3, 0.5)*0.5, pow(r.y*0.5+0.5, 2.0));

    vec3 p = s;
    float dd=0.0;
    float at=0.0;
    float at2 = 0.0;
    for (int i=0; i<MARCH_STEPS; ++i) {
        float d=map(p) * dither;
        if (d<0.001) {
            vec3 n=norm(p);
            float fog = clamp(1.0-dd*0.018, 0.0, 1.0);
            float bot = pow(clamp(-(p.y-10.0)*.1, 0.0, 1.0), 2.0)*fog;

            float aodist = 0.3;
            float ao = clamp(map(p+n*aodist)/aodist, 0.0, 1.0);
            ao = mix(ao, 1.0, 0.3);

            float f = pow(1.0-dot(n, -r), 2.0);
            col += vec3(0.3, 0.4, 0.7)*max(0.0, dot(n, l)) * bot * ao;
            col += 4.0*f * back*2.0 * (-n.y*0.5+0.5) * bot*ao;

            dd = mix(100.0, d, bot);

            break;
        }
        if (dd>100.0) {
            dd=100.0;
            break;
        }
        p+=d*r;
        dd+=d;

        at += exp(-length(tar-p)*7.0);
        at2 += exp(-d*0.1)*0.05;
    }

    col += vec3(1, 0.3, 0.5) * at * 3.3;
    col += vec3(1, 0.3, 0.5) * at2 * 0.2;
    col += back*0.05 * exp(dd*0.05);

    fragColor = vec4(col, 1);
}
// --------[ Original ShaderToy ends here ]---------- //



void main(void)
{
    mainImage(outColor, gl_FragCoord.xy);
}
