#version 300 es
precision mediump float;

in vec2 v_texCoord;

// 环境光(Ambient Lighting)
in vec3 ambient;
// 散射光( Diffuse Lighting)
in vec3 diffuse;
// 镜面光( Specular Lighting)
in vec3 specular;


layout(location = 0) out vec4 outColor;

uniform sampler2D s_TextureMap;

void main()
{
    vec4 objectColor = texture(s_TextureMap, v_texCoord);
    // 冯氏光照模型(Phong Lighting Model)便是其中常用的一个光照模型，它由三种元素光组成，分别是环境光(Ambient Lighting)、散射光(Diffuse Lighting)及镜面光(Specular Lighting)。
    vec3 phongLightingModel = ambient + diffuse + specular;
    vec3 finalColor = phongLightingModel * vec3(objectColor);
    outColor = vec4(finalColor, 1.0);
}