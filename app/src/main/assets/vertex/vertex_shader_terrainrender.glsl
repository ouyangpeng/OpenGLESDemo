#version 300 es                                      
uniform mat4 u_mvpMatrix;
uniform vec3 u_lightDirection;

layout(location = 0) in vec4 a_position;

uniform sampler2D s_texture;

out vec4 v_color;

void main()
{
    // compute vertex normal from height map
    float hxl = textureOffset(s_texture, a_position.xy, ivec2(-1, 0)).w;
    float hxr = textureOffset(s_texture, a_position.xy, ivec2(1, 0)).w;
    float hyl = textureOffset(s_texture, a_position.xy, ivec2(0, -1)).w;
    float hyr = textureOffset(s_texture, a_position.xy, ivec2(0, 1)).w;

    vec3 u = normalize(vec3(0.05, 0.0, hxr-hxl));
    vec3 v = normalize(vec3(0.0, 0.05, hyr-hyl));
    vec3 normal = cross(u, v);

    // compute diffuse lighting
    float diffuse = dot(normal, u_lightDirection);
    v_color = vec4(vec3(diffuse), 1.0);

    // get vertex position from height map
    float h = texture (s_texture, a_position.xy).w;
    vec4 v_position = vec4 (a_position.xy, h/2.5, a_position.w);
    gl_Position = u_mvpMatrix * v_position;
}