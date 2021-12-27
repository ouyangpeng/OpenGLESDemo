#version 300 es
precision mediump float;

// 原来的着色器地址： https://gl-transitions.com/editor/InvertedPageCurl


in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
//一个取值在 0.0~1.0 的（uniform）渐变量
uniform float u_offset;
uniform vec2 u_texSize;



const float MIN_AMOUNT = -0.16;
const float MAX_AMOUNT = 1.5;

const float PI = 3.141592653589793;

const float scale = 512.0;
const float sharpness = 3.0;

const float cylinderRadius = 1.0 / PI / 2.0;

float amount = 0.0;
float cylinderCenter = 0.0;
float cylinderAngle = 0.0;

vec3 hitPoint(float hitAngle, float yc, vec3 point, mat3 rrotation)
{
    float hitPoint = hitAngle / (2.0 * PI);
    point.y = hitPoint;
    return rrotation * point;
}

vec4 antiAlias(vec4 color1, vec4 color2, float distanc)
{
    distanc *= scale;
    if (distanc < 0.0) return color2;
    if (distanc > 2.0) return color1;
    float dd = pow(1.0 - distanc / 2.0, sharpness);
    return ((color2 - color1) * dd) + color1;
}

float distanceToEdge(vec3 point)
{
    float dx = abs(point.x > 0.5 ? 1.0 - point.x : point.x);
    float dy = abs(point.y > 0.5 ? 1.0 - point.y : point.y);
    if (point.x < 0.0) dx = -point.x;
    if (point.x > 1.0) dx = point.x - 1.0;
    if (point.y < 0.0) dy = -point.y;
    if (point.y > 1.0) dy = point.y - 1.0;
    if ((point.x < 0.0 || point.x > 1.0) && (point.y < 0.0 || point.y > 1.0)) return sqrt(dx * dx + dy * dy);
    return min(dx, dy);
}

vec4 seeThrough(float yc, vec2 p, mat3 rotation, mat3 rrotation)
{
    float hitAngle = PI - (acos(yc / cylinderRadius) - cylinderAngle);
    vec3 point = hitPoint(hitAngle, yc, rotation * vec3(p, 1.0), rrotation);
    if (yc <= 0.0 && (point.x < 0.0 || point.y < 0.0 || point.x > 1.0 || point.y > 1.0))
    {
        return texture(u_texture1, p);
    }

    if (yc > 0.0) return texture(u_texture0, p);

    vec4 color = texture(u_texture0, point.xy);
    vec4 tcolor = vec4(0.0);

    return antiAlias(color, tcolor, distanceToEdge(point));
}

vec4 seeThroughWithShadow(float yc, vec2 p, vec3 point, mat3 rotation, mat3 rrotation)
{
    float shadow = distanceToEdge(point) * 30.0;
    shadow = (1.0 - shadow) / 3.0;

    if (shadow < 0.0) shadow = 0.0; else shadow *= amount;

    vec4 shadowColor = seeThrough(yc, p, rotation, rrotation);
    shadowColor.r -= shadow;
    shadowColor.g -= shadow;
    shadowColor.b -= shadow;

    return shadowColor;
}

vec4 backside(float yc, vec3 point)
{
    vec4 color = texture(u_texture0, point.xy);
    float gray = (color.r + color.b + color.g) / 15.0;
    gray += (8.0 / 10.0) * (pow(1.0 - abs(yc / cylinderRadius), 2.0 / 10.0) / 2.0 + (5.0 / 10.0));
    color.rgb = vec3(gray);
    return color;
}

vec4 behindSurface(vec2 p, float yc, vec3 point, mat3 rrotation)
{
    float shado = (1.0 - ((-cylinderRadius - yc) / amount * 7.0)) / 6.0;
    shado *= 1.0 - abs(point.x - 0.5);

    yc = (-cylinderRadius - cylinderRadius - yc);

    float hitAngle = (acos(yc / cylinderRadius) + cylinderAngle) - PI;
    point = hitPoint(hitAngle, yc, point, rrotation);

    if (yc < 0.0 && point.x >= 0.0 && point.y >= 0.0 && point.x <= 1.0 && point.y <= 1.0 && (hitAngle < PI || amount > 0.5))
    {
        shado = 1.0 - (sqrt(pow(point.x - 0.5, 2.0) + pow(point.y - 0.5, 2.0)) / (71.0 / 100.0));
        shado *= pow(-yc / cylinderRadius, 3.0);
        shado *= 0.5;
    }
    else
    {
        shado = 0.0;
    }
    return vec4(texture(u_texture1, p).rgb - shado, 1.0);
}

vec4 transition(vec2 p) {

    const float angle = 100.0 * PI / 180.0;
    float c = cos(-angle);
    float s = sin(-angle);

    mat3 rotation = mat3(
    c, s, 0,
    -s, c, 0,
    -0.801, 0.8900, 1
    );
    c = cos(angle);
    s = sin(angle);

    mat3 rrotation = mat3(
    c, s, 0,
    -s, c, 0,
    0.98500, 0.985, 1
    );

    vec3 point = rotation * vec3(p, 1.0);

    float yc = point.y - cylinderCenter;

    if (yc < -cylinderRadius)
    {
        // Behind surface
        return behindSurface(p, yc, point, rrotation);
    }

    if (yc > cylinderRadius)
    {
        // Flat surface
        return texture(u_texture0, p);
    }

    float hitAngle = (acos(yc / cylinderRadius) + cylinderAngle) - PI;

    float hitAngleMod = mod(hitAngle, 2.0 * PI);
    if ((hitAngleMod > PI && amount < 0.5) || (hitAngleMod > PI/2.0 && amount < 0.0))
    {
        return seeThrough(yc, p, rotation, rrotation);
    }

    point = hitPoint(hitAngle, yc, point, rrotation);

    if (point.x < 0.0 || point.y < 0.0 || point.x > 1.0 || point.y > 1.0)
    {
        return seeThroughWithShadow(yc, p, point, rotation, rrotation);
    }

    vec4 color = backside(yc, point);

    vec4 otherColor;
    if (yc < 0.0)
    {
        float shado = 1.0 - (sqrt(pow(point.x - 0.5, 2.0) + pow(point.y - 0.5, 2.0)) / 0.71);
        shado *= pow(-yc / cylinderRadius, 3.0);
        shado *= 0.5;
        otherColor = vec4(0.0, 0.0, 0.0, shado);
    }
    else
    {
        otherColor = texture(u_texture0, p);
    }

    color = antiAlias(color, otherColor, cylinderRadius - abs(yc));

    vec4 cl = seeThroughWithShadow(yc, p, point, rotation, rrotation);
    float dist = distanceToEdge(point);

    return antiAlias(color, cl, dist);
}

void main()
{
    amount = u_offset * (MAX_AMOUNT - MIN_AMOUNT) + MIN_AMOUNT;
    cylinderCenter = amount;
    cylinderAngle = 2.0 * PI * amount;

    outColor = transition(v_texCoord);
}
