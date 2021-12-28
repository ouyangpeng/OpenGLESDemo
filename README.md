# OpenGL ES Demo
主要是将下面来源的特效，自己手动敲一遍，熟悉OpenGL ES相关API和GLSL语法以及C++语法
1. 《OpenGL ES 3.0编程指南》 这本书
2. [【Learn OpenGL ES】 ](http://www.learnopengles.com/) 网站的一些示例 
3.  字节流动的  [NDK_OpenGLES_3_0 项目代码](https://github.com/githubhaohao/NDK_OpenGLES_3_0)  
4. 部分[ShaderToy](https://www.shadertoy.com/)网站和 [GLSL SANDBOX](https://glslsandbox.com/) 网站上的特效移植到android端
5. 转场动画[GLTransitions](https://gl-transitions.com/gallery)网站上的特效移植到android端

# 相关链接
关于 OpenGL ES的相关知识，
可以参考下面的博客查看：

1. [字节卷动 的 OpenGL ES 专栏](https://blog.csdn.net/ouyang_peng/category_11380480.html)
2. [字节流动 的 Android OpenGLES 3.0 专栏](https://blog.csdn.net/kennethdroid/category_9120765.html)
3. [【Learn OpenGL ES】 ](http://www.learnopengles.com/)
4. [ShaderToy](https://www.shadertoy.com/)
5. [GLSL SANDBOX](https://glslsandbox.com/) 
6. [转场动画 https://gl-transitions.com/gallery](https://gl-transitions.com/gallery)



# 案例效果展示

## 1. 展示一个基本的红色三角形

颜色在片段着色器写死的红色

![1640693434242](.\pics\triangle.png)

## 2. 展示一个基本的蓝色三角形

颜色由glVertexAttrib4fv传给片段着色器

![1640693559272](.\pics\triangle2.png)

## 3. 展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形

使用了顶点缓冲对象(Vertex Buffer Objects, VBO) 和 EBO 技术

![1640693675439](.\pics\triangle3.png)

## 4. 展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形

使用了顶点缓冲对象(Vertex Buffer Objects, VBO) 、 EBO 和 映射缓冲区对象(Map Buffer) 技术

![1640693675439](.\pics\triangle3.png)



## 5. 展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形

使用了 VBO 、 EBO 和 VAO 技术

![1640693675439](.\pics\triangle3.png)



## 6. 对比 两个基本的由红、绿、蓝三种颜色绘制而成的三角形

一个使用VBO绘制，另外一个不使用VBO绘制

![1640693823560](.\pics\triangle4.png)

## 7. 展示一个在不停旋转的红色立方体

使用到了MVP矩阵（模型矩阵，观察矩阵和投影矩阵）技术

![](.\pics\cube.gif)


## 8. 展示一个简单的2D Texture 纹理
学习如何绘制一个2D纹理

![1640694310959](.\pics\texture_2d.png)

## 9. 展示一个立方体贴图Cubemap
学习如何绘制一个立方体贴图Cubemap

![1640694372144](.\pics\cubemap.gif)

## 10. 展示mip贴图(mipmapping)
学习纹理过滤和mip贴图(mipmapping)知识

![1640694492499](.\pics\mipmap2d.gif)

## 11. 对比 三种不同纹理包装模式
学习GL_REPEAT、GL_CLAMP_TO_EDGE、GL_MIRRORED_REPEAT三种不同纹理包装模式
![1640694582671](.\pics\texture_wrap.png)

