#version 300 es
precision highp float;

/*
 * Original shader from: https://www.shadertoy.com/view/wtcBWf
 */

layout(location = 0) out vec4 outColor;

// glslsandbox uniforms
uniform float iTime;
uniform vec2 iResolution;

// --------[ Original ShaderToy begins here ]---------- //

vec2 rot(vec2 a, float c){
    float l = length(a);
    a/=l;
    float ang = (a.y<0.)?acos(a.x):2.*3.14159 - acos(a.x);
    ang += c;
    return vec2(l*cos(ang), l*sin(ang));
}

float DE(vec3 p0){
    vec4 p = vec4(p0, 1.);
    for(int i = 0; i < 20; i++){

        p*=5.2 + sin(iTime)*1.4;
        p.xyz = mod(p.xyz-1.5, 3.)-1.5;
        p.xyz -= vec3(0.4,0.2,0.1)*0.5;
        p/=dot(p.xyz,p.xyz);
    }

    return length(p.xyz)/p.w;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;
    vec3 a = vec3(0.2,0.5,0.3);
    vec3 b = vec3(0.6,0.2,0.1);
    vec3 c = vec3(0.2,0.1,0.4);
    vec3 d = vec3(0.9,0.5,0.5);
    vec3 color = a + b*cos(6.28318*(c*uv.y+d + iTime));



    uv = uv * 2.0 - 1.0;
    uv.x *= 1.3;
    uv.y /= 1.3;

    uv = abs(uv);
    uv = rot(uv, iTime*0.4);

    uv *= 10.;
    float dist = DE((mod(vec3(uv.x, uv.y,1.), 12.)-6.)/20.)*20.;
    vec3 col;
    //if(dist < 0.01){
    //col = vec3(1.);
    // }
    col+=dist*0.9;
    col-=color;

    // Output to screen
    fragColor = vec4(col,1.0);
}
// --------[ Original ShaderToy ends here ]---------- //



void main(void)
{
    mainImage(outColor, gl_FragCoord.xy);
}
