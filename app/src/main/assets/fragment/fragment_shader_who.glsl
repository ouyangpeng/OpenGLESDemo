#version 300 es
precision highp float;

// https://www.shadertoy.com/view/MdfXWX

layout(location = 0) out vec4 outColor;

uniform vec2 u_screenSize;
uniform float u_time;

#define PI2 6.28318530718
#define RES 0.02
#define trunc(a) float(int(a))

float n2f(float note)
{
    return 55.0*pow(2.0,(note-3.0)/12.);
}

float note(float nr)
{
    if (nr<=15.)  return -1.;
    if (nr<=16.)  return 31.;
    if (nr<=17.)  return 56.;

    if (nr<=22.)  return 55.;
    if (nr<=23.)  return -1.;

    if (nr<=24.)  return 58.;
    if (nr<=25.)  return 42.;
    if (nr<=30.)  return 43.;

    if (nr<=33.)  return -1.;

    if (nr<=34.5)  return 43.;
    if (nr<=35.5)  return 39.;
    if (nr<=37.0)  return 31.;
    if (nr<=39.0)  return 34.;
    if (nr<=39.5)  return 32.;
    if (nr<=40.5)  return 31.;
    if (nr<=41.0)  return -1.;
    if (nr<=42.5)  return 31.;
    if (nr<=43.0)  return 44.;
    if (nr<=46.0)  return 43.;

    return -1.0;
}

vec2 getSample(float time, float tt, float FM)
{
    tt -= mod(tt,RES);

    float note1 = note(tt);
    float note2 = note(tt+0.5);
    if (note1 <0.0)
    return vec2(0.0,50.0);

    float stepper = smoothstep(0.2,0.4,mod(tt,0.5));

    float note = mix(note1,note2,stepper);

    float f = n2f(note);
    float angle = PI2*f*time;
    return vec2(sin(angle+FM*sin(angle*2.)),f);
}

vec3 theramin(float time, float tt)
{
    tt = mod(tt,48.0);
    tt += 1.33;
    float FM = 0.0;
    if (tt>=32.)
    FM = PI2/2.;

    float ssample;
    float col = 0.0;
    float ta = mod(tt-RES/2.0,RES)-RES/2.0;
    float halfSin = RES/4.0;//4.0/frequency;
    if (abs(ta)<halfSin)
    {
        float sample1 = getSample(time,tt-RES/2.0,FM).x;
        float sample2 = getSample(time,tt+RES/2.0,FM).x;
        ssample = mix(sample1,sample2,smoothstep(-halfSin,halfSin,ta));
        if (sample1!=sample2)
        col = 1.0;
    }
    else
    ssample = getSample(time,tt,FM).x;

    return vec3( ssample,col,  getSample(time,tt,FM).y);
}

vec3 DoSound(float time)
{
    float tt = time *2.4;
    return theramin(time,tt);
}

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 uv = fragCoord.xy / u_screenSize.xy;
    uv -= 0.5;

    float freq = DoSound(u_time-0.0125).z;

    float timePos = u_time-mod(u_time,1./freq*1.0033)-0.0125+uv.x/45.0;

    vec4 iMouse = vec4(100,100,0,0);

    if (iMouse.z>0.0)
    timePos = uv.x*0.01*iMouse.y/100.0+5.5+iMouse.x/250.0;

    vec3 snd = DoSound(timePos)*0.4;;

    vec3 color = 1.0-vec3(smoothstep(0.004,0.06,distance(vec2(uv.x,snd.x),uv)));
    color.b = snd.y;
    outColor = vec4(color,1.0);
}