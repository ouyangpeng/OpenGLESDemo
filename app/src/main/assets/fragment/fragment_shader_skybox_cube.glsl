#version 300 es
// 天空盒内物体反射的原理图： https://img-blog.csdnimg.cn/20191109204426165.png
// 其中 I 表示观察方向向量，通过当前顶点坐标减去相机位置（观察者）坐标计算得出；
// N 表示物体的法线向量，
// R 为反射向量，通过使用 GLSL 的内建函数 reflect 计算得出反射向量 R。
// 最后，以反射向量 R 作为方向向量对立方图进行索采样，返回采样结果（一个对应反射环境的颜色值）。最后的效果看起来就像物体反射了天空盒。

precision mediump float;
// 代表3D纹理坐标的方向向量
in vec3 v_texCoord;

// 法线向量
in vec3 v_normal;

layout(location = 0) out vec4 outColor;

// 立方体贴图的纹理采样器
uniform samplerCube s_SkyBox;
uniform vec3 u_cameraPos;

void main()
{
    // 折射率
    //折射率决定了材质中光线弯曲的程度，每个材质都有自己的折射率。一些最常见的折射率可以在下表中找到：

    //材质	折射率
    //空气	1.00
    //水	1.33
    //冰	1.309
    //玻璃	1.52
    //钻石	2.42

    // 在我们的例子中，光线/视线从空气进入玻璃（如果我们假设箱子是玻璃制的） 比值为 1.00 / 1.52
    float ratio = 1.00 / 1.52;

    //  I 表示观察方向向量，通过当前顶点坐标减去相机位置（观察者）坐标计算得出
    vec3 I = normalize(v_texCoord - u_cameraPos);
    //  R 表示反射向量，通过使用 GLSL 的内建函数 reflect 计算得出反射向量 R

    vec3 R = reflect(I, normalize(v_normal));
    //vec3 R = refract(I, normalize(v_normal), ratio);

    //折射 以反射向量 R 作为方向向量对立方图进行索采样，返回采样结果（一个对应反射环境的颜色值）。最后的效果看起来就像物体反射了天空盒。
    outColor = texture(s_SkyBox, R);
}
