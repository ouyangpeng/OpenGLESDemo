# 一、介绍
最近在学习OpenGL ES 3.0 相关技术，写了一个Demo工程，记录学习。
## 1.1 项目地址： 
+ [https://github.com/ouyangpeng/OpenGLESDemo](https://github.com/ouyangpeng/OpenGLESDemo)
![在这里插入图片描述](https://img-blog.csdnimg.cn/7b8061e9d8934f1e956aed9fb6c9199f.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_20,color_FFFFFF,t_70,g_se,x_16)

## 1.2 项目介绍

主要是将下面来源的特效，自己手动敲一遍，熟悉OpenGL ES相关API和GLSL语法以及C++语法
1. 《OpenGL ES 3.0编程指南》 这本书
2. [【Learn OpenGL ES】 ](http://www.learnopengles.com/) 网站的一些示例 
3.  字节流动的  [NDK_OpenGLES_3_0 项目代码](https://github.com/githubhaohao/NDK_OpenGLES_3_0)  
4. 部分[ShaderToy](https://www.shadertoy.com/)网站和 [GLSL SANDBOX](https://glslsandbox.com/) 网站上的特效移植到android端
5. 转场动画[GLTransitions](https://gl-transitions.com/gallery)网站上的特效移植到android端

## 1.3 相关链接


关于 OpenGL ES的相关知识，
可以参考下面的博客查看：

1. [字节卷动 的 OpenGL ES 专栏](https://blog.csdn.net/ouyang_peng/category_11380480.html)
2. [字节流动 的 Android OpenGLES 3.0 专栏](https://blog.csdn.net/kennethdroid/category_9120765.html)
3. [【Learn OpenGL ES】 ](http://www.learnopengles.com/)
4. [ShaderToy](https://www.shadertoy.com/)
5. [GLSL SANDBOX](https://glslsandbox.com/) 
6. [转场动画 https://gl-transitions.com/gallery](https://gl-transitions.com/gallery)

# 二、案例效果展示

## 1. 展示一个基本的红色三角形

颜色在片段着色器写死的红色

![在这里插入图片描述](https://img-blog.csdnimg.cn/c8855be307924bb4882155e7a37353a5.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_16,color_FFFFFF,t_70,g_se,x_16)


## 2. 展示一个基本的蓝色三角形

颜色由glVertexAttrib4fv传给片段着色器

![在这里插入图片描述](https://img-blog.csdnimg.cn/a19b101f6e6a49b7a7edfb5f866c9c2e.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_16,color_FFFFFF,t_70,g_se,x_16)


## 3. 展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形

使用了顶点缓冲对象(Vertex Buffer Objects, VBO) 和 EBO 技术

![在这里插入图片描述](https://img-blog.csdnimg.cn/4a5540ef420a4b49badcff84b6910daa.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_15,color_FFFFFF,t_70,g_se,x_16)


## 4. 展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形

使用了顶点缓冲对象(Vertex Buffer Objects, VBO) 、 EBO 和 映射缓冲区对象(Map Buffer) 技术

![在这里插入图片描述](https://img-blog.csdnimg.cn/4a5540ef420a4b49badcff84b6910daa.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_15,color_FFFFFF,t_70,g_se,x_16)



## 5. 展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形

使用了 VBO 、 EBO 和 VAO 技术

![在这里插入图片描述](https://img-blog.csdnimg.cn/4a5540ef420a4b49badcff84b6910daa.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_15,color_FFFFFF,t_70,g_se,x_16)



## 6. 对比 两个基本的由红、绿、蓝三种颜色绘制而成的三角形

一个使用VBO绘制，另外一个不使用VBO绘制

![\[外链图片转存失败,源站可能有防盗链机制,建议将图片保存下来直接上传(img-5yRMP2e6-1640695721701)( https://github.com/ouyangpeng/OpenGLESDemo/blob/master/pics/triangle4.png)\]](https://img-blog.csdnimg.cn/7238450054674376ac91d15e0d9c7c7c.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_16,color_FFFFFF,t_70,g_se,x_16)


## 7. 展示一个在不停旋转的红色立方体

使用到了MVP矩阵（模型矩阵，观察矩阵和投影矩阵）技术

![在这里插入图片描述](https://img-blog.csdnimg.cn/62cb30070c754fce8399f883f6cf7f90.gif#pic_center)




## 8. 展示一个简单的2D Texture 纹理
学习如何绘制一个2D纹理

![在这里插入图片描述](https://img-blog.csdnimg.cn/f07fb98351ed42fe89356b97298d18b7.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_16,color_FFFFFF,t_70,g_se,x_16)


## 9. 展示一个立方体贴图Cubemap
学习如何绘制一个立方体贴图Cubemap

![](https://img-blog.csdnimg.cn/df890e183a014029aef365a6f0c0ba71.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_16,color_FFFFFF,t_70,g_se,x_16)


## 10. 展示mip贴图(mipmapping)
学习纹理过滤和mip贴图(mipmapping)知识
![在这里插入图片描述](https://img-blog.csdnimg.cn/541aac2641884cd8b69b588ae9460842.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)


## 11. 对比 三种不同纹理包装模式
![](https://img-blog.csdnimg.cn/02dec90720df4d0cb2ba03101dfb0823.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)


## 12. 展示一个多重纹理
学习如何使用多重纹理
![在这里插入图片描述](https://img-blog.csdnimg.cn/c0a5a068ccc94d0398a2ddcc1097cd0c.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_13,color_FFFFFF,t_70,g_se,x_16)
## 13. 展示一个不停变化位置的爆炸效果
学习如何用点精灵渲染粒子爆炸效果


![在这里插入图片描述](https://img-blog.csdnimg.cn/3f4d89c07b63414e9ab860d8bbb1c1f3.gif#pic_center)
## 14. 使用变化反馈的粒子系统实现源源不断喷射的爆炸效果
学习 使用变化反馈的粒子系统


![在这里插入图片描述](https://img-blog.csdnimg.cn/d0cd891a23c94e77975918234a12833c.gif#pic_center)
## 15. 展示 3D纹理噪音
学习实现3D纹理噪音

![在这里插入图片描述](https://img-blog.csdnimg.cn/3c00a34a7ea24cde97dfd3119f30d128.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)
## 16. 展示 多重渲染技术同时渲染到4个目标
学习 MRT(多重渲染目标) 技术

![在这里插入图片描述](https://img-blog.csdnimg.cn/d01e325ea74a46aeb087b5e12e454592.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)
## 17 展示一幅立体地形画
学习 用顶点纹理读取渲染地形 技术
![在这里插入图片描述](https://img-blog.csdnimg.cn/37e7183de25143eda688f9036c72bd8f.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)

## 18 展示 深度纹理的阴影 效果图
使用深度纹理的阴影
![在这里插入图片描述](https://img-blog.csdnimg.cn/b1db664aa92a48d8a80cb1bc80f82529.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)
## 19 展示 三个彩色三角形 不停旋转的效果
学习绘制三角形以及使用矩阵变化进行旋转的效果

![请添加图片描述](https://img-blog.csdnimg.cn/5415e7ee450e4378ab475f88b48284fe.gif)
## 20 展示 一个点和五个立方体 不停旋转的效果
学习 使用两个程序对象分别绘制点和立方体，并且使用矩阵变化进行旋转的效果
![请添加图片描述](https://img-blog.csdnimg.cn/2d351d1ca61d4f04ad456bc101ffcf83.gif)
## 21 展示 一个点和五个立方体 不停旋转的效果
这是在上面案例的基础上添加每像素照明

![请添加图片描述](https://img-blog.csdnimg.cn/b08f795f400942e2af86915e948818e5.gif)
## 22 展示 一个点和五个立方体 不停旋转的效果 , 并且立方体有基本的Texture纹理
这是在上面案例的基础上添加的基本纹理

![请添加图片描述](https://img-blog.csdnimg.cn/116a476fb90040f1aad3f06a64b675c7.gif)
## 23 展示 一个点和五个立方体 不停旋转的效果 , 然后切换混合模式的效果
展示触摸屏幕切换 混合模式的效果
![请添加图片描述](https://img-blog.csdnimg.cn/419bdceae3744d54b2674d3d1d1a88ab.gif)
## 24 Texture纹理 的不同过滤模式
本课程将介绍OpenGL ES中可用的不同纹理过滤模式
![请添加图片描述](https://img-blog.csdnimg.cn/7811c4a46f134467a2169ef2e68a8625.gif)
## 25 使用 顶点缓冲区对象（VBO）来绘制多个立方体
学到的知识点：立方体的数量可以改变，可以切换是否使用VBOs,Native可以调用Java方法
![请添加图片描述](https://img-blog.csdnimg.cn/0f9498742fae4c31a783e8054618ab80.gif)
## 26 使用 IBO 来绘制
本课程将介绍索引缓冲区对象（IBO）
![请添加图片描述](https://img-blog.csdnimg.cn/85ca3ec864424556a758b8f9293a8119.gif)
## 27 纹理映射
纹理映射就是通过为图元的顶点坐标指定恰当的纹理坐标，通过纹理坐标在纹理图中选定特定的纹理区域，最后通过纹理坐标与顶点的映射关系，将选定的纹理区域映射到指定图元上

![在这里插入图片描述](https://img-blog.csdnimg.cn/3cc977798e37432bb23970a8ac3f499e.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)

## 28 YUV 渲染
YUV 图不能直接用于显示，需要转换为 RGB 格式，而 YUV 转 RGB 是一个逐像素处理的耗时操作，在 CPU 端进行转换效率过低，这时正好可以利用 GPU 强大的并行处理能力来实现 YUV 到 RGB 的转换

这个案例学习如何渲染YUV图像
![在这里插入图片描述](https://img-blog.csdnimg.cn/8a004c1be7e8456288e1f3c3349278c8.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)
## 29 使用 FBO 技术，实现将彩色照片渲染成黑白照片
学习 FBO(帧缓冲区对象) 进行离屏渲染
![在这里插入图片描述](https://img-blog.csdnimg.cn/8452b37330264b018bb37e09dff46aeb.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)
## 30 使用 EGL渲染 来实现 马赛克、网格、旋转、边缘、放大、形变等效果
学习 使用EGL渲染 技术
![请添加图片描述](https://img-blog.csdnimg.cn/e933479aee2d43fb8b8bd81b67c5c5ab.gif)
## 31 对纹理进行缩放和旋转 
单指可以对图片进行旋转，双指可以进行缩放
![请添加图片描述](https://img-blog.csdnimg.cn/3fccdb4182ac4fbc9ab200816cc28499.gif)
## 32 学习光照基础
学习冯氏光照模型(Phong Lighting Model)，它由三种元素光组成，分别是环境光(Ambient Lighting)、散射光(Diffuse Lighting)及镜面光(Specular Lighting)
![请添加图片描述](https://img-blog.csdnimg.cn/c1d7eaa0e8e14eb294671008603f92b3.gif)
## 33 学习多光照实例
学习冯氏光照模型(Phong Lighting Model)的多光照实例
![请添加图片描述](https://img-blog.csdnimg.cn/4a8281d9d0994a1a99b3beaa6751115f.gif)
## 34 学习 OpenGL ES 实例化（Instancing）
OpenGL ES 实例化（Instancing）是一种只调用一次渲染函数就能绘制出很多物体的技术，可以实现将数据一次性发送给 GPU ，告诉 OpenGL ES 使用一个绘制函数，将这些数据绘制成多个物体

![请添加图片描述](https://img-blog.csdnimg.cn/a16b6fb9d1c249c6b5e913fc29062758.gif)
## 35 学习 OpenGL ES 模板测试
OpenGL ES 模板测试与深度测试类似，主要作用是利用模板缓冲区(Stencil Buffer)所保存的模板值决定当前片段是否被丢弃，且发生于深度测试之前。

![请添加图片描述](https://img-blog.csdnimg.cn/b15fb53848f34ddda7f08bd14f7628b6.gif)
 ## 36 学习 OpenGL ES 混合
 OpenGL ES 混合本质上是将 2 个片元的颜色进行调和，产生一个新的颜色。
 OpenGL ES 混合发生在片元通过各项测试之后，准备进入帧缓冲区的片元和原有的片元按照特定比例加权计算出最终片元的颜色值，不再是新（源）片元直接覆盖缓冲区中的（目标）片元。
 ![在这里插入图片描述](https://img-blog.csdnimg.cn/2641556d0f8c430791838783bd6ba645.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5a2X6IqC5Y235Yqo,size_18,color_FFFFFF,t_70,g_se,x_16)
## 37 展示一个不停变化位置的粒子效果
再次实现一个粒子效果，不一样的实现方式
![请添加图片描述](https://img-blog.csdnimg.cn/83ad1b36beee4b01abfe36e80246e5cd.gif)
## 38 展示一个立方体贴图（天空盒）,可以旋转 
使用立方体贴图实现一个天空盒效果
![请添加图片描述](https://img-blog.csdnimg.cn/1b79163ec585496c99adc659c9c6a536.gif)
## 39 展示一个PBO示例
OpenGL PBO（Pixel Buffer Object），被称为像素缓冲区对象，主要被用于异步像素传输操作。PBO 仅用于执行像素传输，不连接到纹理，且与 FBO （帧缓冲区对象）无关。
![请添加图片描述](https://img-blog.csdnimg.cn/4c1c8b5a718e456481c4945b5c62ddaa.gif)

未完待续... 



