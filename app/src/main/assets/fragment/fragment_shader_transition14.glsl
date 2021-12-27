#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/Swirl

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;


vec4 transition(vec2 UV)
{
    float Radius = 1.0;

    float T = u_offset;

    UV -= vec2( 0.5, 0.5 );

    float Dist = length(UV);

    if ( Dist < Radius )
    {
        float Percent = (Radius - Dist) / Radius;
        float A = ( T <= 0.5 ) ? mix( 0.0, 1.0, T/0.5 ) : mix( 1.0, 0.0, (T-0.5)/0.5 );
        float Theta = Percent * Percent * A * 8.0 * 3.14159;
        float S = sin( Theta );
        float C = cos( Theta );
        UV = vec2( dot(UV, vec2(C, -S)), dot(UV, vec2(S, C)) );
    }
    UV += vec2( 0.5, 0.5 );

    vec4 C0 = texture(u_texture0,UV);
    vec4 C1 = texture(u_texture1,UV);

    return mix( C0, C1, T );
}



void main()
{
    outColor = transition(v_texCoord);
}