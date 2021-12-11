#version 300 es
precision mediump float;

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
//当前片段法向量
layout(location = 2) in vec3 a_normal;

uniform mat4 u_MVPMatrix;
uniform mat4 u_ModelMatrix;
//光源位置
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

out vec2 v_texCoord;

// 环境光(Ambient Lighting)
out vec3 ambient;
// 散射光( Diffuse Lighting)
out vec3 diffuse;
// 镜面光( Specular Lighting)
out vec3 specular;

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    // 当前片段光源照射方向向量
    vec3 fragPos = vec3(u_ModelMatrix * a_position);

    // ==================  Ambient  环境光
    // 常量环境因子
    float ambientStrength = 0.1;
    // 环境光强度
    ambient = ambientStrength * lightColor;


    // ==================  Diffuse 散射光
    // 散射光最终强度 = 材质反射系数 × 散射光强度 × max(cos(入射角)，0)
    // 其中入射角表示：当前片段光源照射方向与法向量之间的夹角。

    // 材质反射系数
    float diffuseStrength = 0.5;
    // 归一化
    vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));
    // 当前片光源照射方向向量
    vec3 lightDir = normalize(lightPos - fragPos);
    // dot表示两个向量的点乘
    float diff = max(dot(unitNormal, lightDir), 0.0);
    // 散射光 最终强度
    diffuse = diffuseStrength * diff * lightColor;

    // ==================  Specular 镜面光
    //镜面光最终强度 = 材质镜面亮度因子 × 镜面光强度 × max(cos(反射光向量与视线方向向量夹角)，0)
    //修正后的模型也可表示为：
    //镜面光最终强度 = 材质镜面亮度因子 × 镜面光强度 × max(cos(半向量与法向量夹角)，0)
    //其中半向量为镜面反射光向量与视线方向向量（从片段到观察者）的半向量。

    float specularStrength = 0.9;
    // 视线方向向量
    vec3 viewDir = normalize(viewPos - fragPos);
    // 镜面反射光向量
    vec3 reflectDir = reflect(-lightDir, unitNormal);

    // 视线方向向量与镜面反射光向量点乘的16次幂，这个16是高光的发光值(Shininess)。
    // 一个物体的发光值越高，反射光的能力越强，散射得越少，高光点越小。
    float spec = pow(max(dot(unitNormal, reflectDir), 0.0), 16.0);
    //镜面光最终强度
    specular = specularStrength * spec * lightColor;
    v_texCoord = a_texCoord;
}