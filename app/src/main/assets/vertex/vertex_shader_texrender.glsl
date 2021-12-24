#version 300 es
layout(location = 0) in vec4 a_position;// <vec2 pos, vec2 tex>

uniform mat4 u_MVPMatrix;

out vec2 v_texCoord;
void main()
{
    gl_Position = u_MVPMatrix * vec4(a_position.xy, 0.0, 1.0);
    v_texCoord = a_position.zw;
}