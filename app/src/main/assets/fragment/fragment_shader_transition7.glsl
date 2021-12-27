#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/BowTieVertical

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;

float check(vec2 p1, vec2 p2, vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle (vec2 pt, vec2 p1, vec2 p2, vec2 p3)
{
    bool b1, b2, b3;
    b1 = check(pt, p1, p2) < 0.0;
    b2 = check(pt, p2, p3) < 0.0;
    b3 = check(pt, p3, p1) < 0.0;
    return ((b1 == b2) && (b2 == b3));
}

bool in_top_triangle(vec2 p){
    vec2 vertex1, vertex2, vertex3;
    vertex1 = vec2(0.5, u_offset);
    vertex2 = vec2(0.5-u_offset, 0.0);
    vertex3 = vec2(0.5+u_offset, 0.0);
    if (PointInTriangle(p, vertex1, vertex2, vertex3))
    {
        return true;
    }
    return false;
}

bool in_bottom_triangle(vec2 p){
    vec2 vertex1, vertex2, vertex3;
    vertex1 = vec2(0.5, 1.0 - u_offset);
    vertex2 = vec2(0.5-u_offset, 1.0);
    vertex3 = vec2(0.5+u_offset, 1.0);
    if (PointInTriangle(p, vertex1, vertex2, vertex3))
    {
        return true;
    }
    return false;
}

float blur_edge(vec2 bot1, vec2 bot2, vec2 top, vec2 testPt)
{
    vec2 lineDir = bot1 - top;
    vec2 perpDir = vec2(lineDir.y, -lineDir.x);
    vec2 dirToPt1 = bot1 - testPt;
    float dist1 = abs(dot(normalize(perpDir), dirToPt1));

    lineDir = bot2 - top;
    perpDir = vec2(lineDir.y, -lineDir.x);
    dirToPt1 = bot2 - testPt;
    float min_dist = min(abs(dot(normalize(perpDir), dirToPt1)), dist1);

    if (min_dist < 0.005) {
        return min_dist / 0.005;
    }
    else  {
        return 1.0;
    };
}


vec4 transition (vec2 uv) {
    if (in_top_triangle(uv))
    {
        if (u_offset < 0.1)
        {
            return texture(u_texture0, uv);
        }
        if (uv.y < 0.5)
        {
            vec2 vertex1 = vec2(0.5, u_offset);
            vec2 vertex2 = vec2(0.5-u_offset, 0.0);
            vec2 vertex3 = vec2(0.5+u_offset, 0.0);
            return mix(
            texture(u_texture0, uv),
            texture(u_texture1, uv),
            blur_edge(vertex2, vertex3, vertex1, uv)
            );
        }
        else
        {
            if (u_offset > 0.0)
            {
                return texture(u_texture1, uv);
            }
            else
            {
                return texture(u_texture0, uv);
            }
        }
    }
    else if (in_bottom_triangle(uv))
    {
        if (uv.y >= 0.5)
        {
            vec2 vertex1 = vec2(0.5, 1.0-u_offset);
            vec2 vertex2 = vec2(0.5-u_offset, 1.0);
            vec2 vertex3 = vec2(0.5+u_offset, 1.0);
            return mix(
            texture(u_texture0, uv),
            texture(u_texture1, uv),
            blur_edge(vertex2, vertex3, vertex1, uv)
            );
        }
        else
        {
            return texture(u_texture0, uv);
        }
    }
    else {
        return texture(u_texture0, uv);
    }
}

void main()
{
    outColor = transition(v_texCoord);
}