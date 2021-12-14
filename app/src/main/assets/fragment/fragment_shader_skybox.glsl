#version 300 es
precision mediump float;

// 注意: 纹理坐标变成了三维方向向量。 代表3D纹理坐标的方向向量
in vec3 v_texCoord;

layout(location = 0) out vec4 outColor;

// 注意: 采样器变成了 samplerCube  立方体贴图的纹理采样器
uniform samplerCube s_SkyBox;

void main()
{
    outColor = texture(s_SkyBox, v_texCoord);
}