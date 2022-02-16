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
    // https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/
    // 冯氏光照模型(Phong Lighting Model)便是其中常用的一个光照模型，它由三种元素光组成，分别是环境光(Ambient Lighting)、散射光(Diffuse Lighting)及镜面光(Specular Lighting)。
    // 环境光照(Ambient Lighting)：即使在黑暗的情况下，世界上通常也仍然有一些光亮（月亮、远处的光），所以物体几乎永远不会是完全黑暗的。为了模拟这个，我们会使用一个环境光照常量，它永远会给物体一些颜色。
    // 漫反射光照(Diffuse Lighting)：模拟光源对物体的方向性影响(Directional Impact)。它是冯氏光照模型中视觉上最显著的分量。物体的某一部分越是正对着光源，它就会越亮。
    // 镜面光照(Specular Lighting)：模拟有光泽物体上面出现的亮点。镜面光照的颜色相比于物体的颜色会更倾向于光的颜色。
    vec3 phongLightingModel = ambient + diffuse + specular;
    vec3 finalColor = phongLightingModel * vec3(objectColor);
    outColor = vec4(finalColor, 1.0);
}