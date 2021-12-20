package com.oyp.openglesdemo.activity

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.util.SparseArray
import android.util.SparseIntArray
import com.oyp.openglesdemo.IMyNativeRendererType
import com.oyp.openglesdemo.lesson.*
import com.oyp.openglesdemo.lesson.lesson7.LessonSevenActivity
import com.oyp.openglesdemo.lesson.lesson8.LessonEightActivity
import java.util.*
import android.view.View
import androidx.recyclerview.widget.DividerItemDecoration

import androidx.recyclerview.widget.RecyclerView
import com.oyp.openglesdemo.R
import androidx.recyclerview.widget.LinearLayoutManager
import com.oyp.openglesdemo.adapter.GLRecyclerAdapter


class MainActivity : Activity() {

    val data: MutableList<Map<String, Any?>> = ArrayList()
    private val typeMapping = SparseIntArray()
    private val activityMapping = SparseArray<Class<out Activity?>>()
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        // Initialize data
        initData()

        val recyclerView = findViewById<View>(R.id.recycler_view) as RecyclerView

        val layoutManager = LinearLayoutManager(this)
        //设置水平滚动
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        recyclerView.layoutManager = layoutManager

        // 设置系统自带的间割线
        recyclerView.addItemDecoration(DividerItemDecoration(this,LinearLayoutManager.VERTICAL))

        val adapter = GLRecyclerAdapter(data,this)
        recyclerView.adapter = adapter
        // 设置点击事件回调接口
        adapter.setOnItemClickListener { position->
            val type = typeMapping[position]
            // 这两个demo没有改造完，用原来的方式启动
            Log.d("MainActivity", "type = $type")
            when (type) {
                IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_SEVEN,
                IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_EIGHT,
                IMyNativeRendererType.SAMPLE_TYPE_KEY_EGL -> {
                    // 取出要启动的Activity
                    val activityToLaunch = activityMapping[position]

                    Log.d("MainActivity", "activityToLaunch = $activityToLaunch")
                    if (activityToLaunch != null) {
                        val launchIntent = Intent(this@MainActivity, activityToLaunch)
                        startActivity(launchIntent)
                    }
                }
                else -> {
                    val launchIntent =
                        Intent(this@MainActivity, NativeRenderActivity::class.java)
                    launchIntent.putExtra(IMyNativeRendererType.RENDER_TYPE, type)
                    startActivity(launchIntent)
                }
            }
        }
    }

    private fun initData() {
        var i = 0
        val item0: MutableMap<String, Any?> = HashMap()
        item0[ITEM_IMAGE] = R.mipmap.ic_triangle
        item0[ITEM_TITLE] = "展示一个基本的红色三角形"
        item0[ITEM_SUBTITLE] = "颜色在片段着色器写死的红色"
        data.add(item0)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_TRIANGLE)

        val itemHelloTriangle2: MutableMap<String, Any?> = HashMap()
        itemHelloTriangle2[ITEM_IMAGE] = R.mipmap.ic_triangle2
        itemHelloTriangle2[ITEM_TITLE] = "展示一个基本的蓝色三角形"
        itemHelloTriangle2[ITEM_SUBTITLE] = "颜色由glVertexAttrib4fv传给片段着色器"
        data.add(itemHelloTriangle2)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_TRIANGLE2)

        val itemHelloTriangle3: MutableMap<String, Any?> = HashMap()
        itemHelloTriangle3[ITEM_IMAGE] = R.mipmap.ic_triangle3
        itemHelloTriangle3[ITEM_TITLE] = "展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形"
        itemHelloTriangle3[ITEM_SUBTITLE] = "使用了顶点缓冲对象(Vertex Buffer Objects, VBO) 和 EBO 技术"
        data.add(itemHelloTriangle3)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_TRIANGLE3)

        val itemHelloTriangleMapBuffers: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleMapBuffers[ITEM_IMAGE] = R.mipmap.ic_triangle_mapbuffer
        itemHelloTriangleMapBuffers[ITEM_TITLE] = "展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形"
        itemHelloTriangleMapBuffers[ITEM_SUBTITLE] =
            "使用了顶点缓冲对象(Vertex Buffer Objects, VBO) 、 EBO 和 映射缓冲区对象(Map Buffer) 技术"
        data.add(itemHelloTriangleMapBuffers)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TRIANGLE_MAP_BUFFERS)

        val itemHelloTriangleVertexArrayObject: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleVertexArrayObject[ITEM_IMAGE] = R.mipmap.ic_triangle_mapbuffer
        itemHelloTriangleVertexArrayObject[ITEM_TITLE] = "展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形"
        itemHelloTriangleVertexArrayObject[ITEM_SUBTITLE] = "使用了 VBO 、 EBO 和 VAO 技术"
        data.add(itemHelloTriangleVertexArrayObject)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TRIANGLE_VERTEX_ARRAY_OBJECT)

        val itemHelloTriangleVertexBufferObject: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleVertexBufferObject[ITEM_IMAGE] = R.mipmap.ic_triangle_vbo
        itemHelloTriangleVertexBufferObject[ITEM_TITLE] = "对比 两个基本的由红、绿、蓝三种颜色绘制而成的三角形"
        itemHelloTriangleVertexBufferObject[ITEM_SUBTITLE] = "一个使用VBO绘制，另外一个不使用VBO绘制"
        data.add(itemHelloTriangleVertexBufferObject)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TRIANGLE_VERTEX_BUFFER_OBJECT)


        val itemHelloTriangleSimpleVertexShader: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleSimpleVertexShader[ITEM_IMAGE] = R.mipmap.ic_cube
        itemHelloTriangleSimpleVertexShader[ITEM_TITLE] = "展示一个在不停旋转的红色立方体"
        itemHelloTriangleSimpleVertexShader[ITEM_SUBTITLE] = "使用到了MVP矩阵（模型矩阵，观察矩阵和投影矩阵）技术"
        data.add(itemHelloTriangleSimpleVertexShader)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_CUBE_SIMPLE_VERTEX_SHADER)


        val itemSimpleTexture2D: MutableMap<String, Any?> = HashMap()
        itemSimpleTexture2D[ITEM_IMAGE] = R.mipmap.ic_texture_2d
        itemSimpleTexture2D[ITEM_TITLE] = "展示一个简单的2D Texture 纹理 "
        itemSimpleTexture2D[ITEM_SUBTITLE] = "学习如何绘制一个2D纹理"
        data.add(itemSimpleTexture2D)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SIMPLE_TEXTURE_2D)

        val itemSimpleTextureCubemap: MutableMap<String, Any?> = HashMap()
        itemSimpleTextureCubemap[ITEM_IMAGE] = R.mipmap.ic_texture_cubemap
        itemSimpleTextureCubemap[ITEM_TITLE] = " 展示一个立方体贴图Cubemap"
        itemSimpleTextureCubemap[ITEM_SUBTITLE] = "学习如何绘制一个立方体贴图Cubemap"
        data.add(itemSimpleTextureCubemap)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SIMPLE_TEXTURE_CUBE_MAP)

        val itemMipmap2D: MutableMap<String, Any?> = HashMap()
        itemMipmap2D[ITEM_IMAGE] = R.mipmap.ic_mimmap_2d
        itemMipmap2D[ITEM_TITLE] = "展示mip贴图(mipmapping)"
        itemMipmap2D[ITEM_SUBTITLE] = "学习纹理过滤和mip贴图(mipmapping)知识"
        data.add(itemMipmap2D)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_MIPMAP_2D)


        val itemTextureWrap: MutableMap<String, Any?> = HashMap()
        itemTextureWrap[ITEM_IMAGE] = R.mipmap.ic_texture_wrap
        itemTextureWrap[ITEM_TITLE] = "对比 三种不同纹理包装模式"
        itemTextureWrap[ITEM_SUBTITLE] = "学习GL_REPEAT、GL_CLAMP_TO_EDGE、GL_MIRRORED_REPEAT三种不同纹理包装模式"
        data.add(itemTextureWrap)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TEXTURE_WRAP)


        val itemMultiTexture: MutableMap<String, Any?> = HashMap()
        itemMultiTexture[ITEM_IMAGE] = R.mipmap.ic_multi_texture
        itemMultiTexture[ITEM_TITLE] = "展示一个多重纹理"
        itemMultiTexture[ITEM_SUBTITLE] = "学习如何使用多重纹理"
        data.add(itemMultiTexture)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_MULTI_TEXTURE)


        val itemParticleSystem: MutableMap<String, Any?> = HashMap()
        itemParticleSystem[ITEM_IMAGE] = R.mipmap.ic_particle_system
        itemParticleSystem[ITEM_TITLE] = "展示一个不停变化位置的爆炸效果"
        itemParticleSystem[ITEM_SUBTITLE] = "学习如何用点精灵渲染粒子爆炸效果"
        data.add(itemParticleSystem)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM)


        val itemParticleSystemTransformFeed: MutableMap<String, Any?> = HashMap()
        itemParticleSystemTransformFeed[ITEM_IMAGE] = R.mipmap.ic_particle_system_transform_feedback
        itemParticleSystemTransformFeed[ITEM_TITLE] = "使用变化反馈的粒子系统实现源源不断喷射的爆炸效果"
        itemParticleSystemTransformFeed[ITEM_SUBTITLE] = "使用变化反馈的粒子系统"
        data.add(itemParticleSystemTransformFeed)
        typeMapping.put(
            i++,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM_TRANSFORM_FEEDBACK
        )

        val itemNoise3D: MutableMap<String, Any?> = HashMap()
        itemNoise3D[ITEM_IMAGE] = R.mipmap.ic_noise3d
        itemNoise3D[ITEM_TITLE] = "展示 3D纹理噪音 "
        itemNoise3D[ITEM_SUBTITLE] = "学习实现3D纹理噪音"
        data.add(itemNoise3D)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_NOISE3D)

        val itemMRT: MutableMap<String, Any?> = HashMap()
        itemMRT[ITEM_IMAGE] = R.mipmap.ic_mrt
        itemMRT[ITEM_TITLE] = "展示 多重渲染技术同时渲染到4个目标"
        itemMRT[ITEM_SUBTITLE] = "学习 MRT(多重渲染目标) 技术"
        data.add(itemMRT)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_MRT)

        val itemTerrainRender: MutableMap<String, Any?> = HashMap()
        itemTerrainRender[ITEM_IMAGE] = R.mipmap.ic_terrainrender
        itemTerrainRender[ITEM_TITLE] = "展示一幅立体地形画"
        itemTerrainRender[ITEM_SUBTITLE] = "学习 用顶点纹理读取渲染地形 技术"
        data.add(itemTerrainRender)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TERRAIN_RENDER)

        val itemShadows: MutableMap<String, Any?> = HashMap()
        itemShadows[ITEM_IMAGE] = R.mipmap.ic_shadows
        itemShadows[ITEM_TITLE] = "展示 深度纹理的阴影 效果图"
        itemShadows[ITEM_SUBTITLE] = "使用深度纹理的阴影"
        data.add(itemShadows)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADOWS)

        val item1: MutableMap<String, Any?> = HashMap()
        item1[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        item1[ITEM_TITLE] = "展示 三个彩色三角形 不停旋转的效果"
        item1[ITEM_SUBTITLE] = "学习绘制三角形以及使用矩阵变化进行旋转的效果"
        data.add(item1)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_ONE)

        val item2: MutableMap<String, Any> = HashMap()
        item2[ITEM_IMAGE] = R.mipmap.ic_lesson_two
        item2[ITEM_TITLE] = "展示 一个点和五个立方体 不停旋转的效果"
        item2[ITEM_SUBTITLE] = "学习 使用两个程序对象分别绘制点和立方体，并且使用矩阵变化进行旋转的效果"
        data.add(item2)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_TWO)

        val item3: MutableMap<String, Any> = HashMap()
        item3[ITEM_IMAGE] = R.mipmap.ic_lesson_three
        item3[ITEM_TITLE] = "展示 一个点和五个立方体 不停旋转的效果"
        item3[ITEM_SUBTITLE] = "这是在第二课的基础上添加每像素照明。"
        data.add(item3)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_THREE)

        val item4: MutableMap<String, Any> = HashMap()
        item4[ITEM_IMAGE] = R.mipmap.ic_lesson_four
        item4[ITEM_TITLE] = "展示 一个点和五个立方体 不停旋转的效果 , 并且立方体有基本的Texture纹理"
        item4[ITEM_SUBTITLE] = "这是在第三课的基础上添加的基本纹理。"
        data.add(item4)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_FOUR)

        val item5: MutableMap<String, Any> = HashMap()
        item5[ITEM_IMAGE] = R.mipmap.ic_lesson_five
        item5[ITEM_TITLE] = "展示触摸屏幕切换 混合模式的效果"
        item5[ITEM_SUBTITLE] = "本课程将介绍OpenGL混合的基础知识"
        data.add(item5)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_FIVE)

        val item6: MutableMap<String, Any> = HashMap()
        item6[ITEM_IMAGE] = R.mipmap.ic_lesson_six
        item6[ITEM_TITLE] = "展示 Texture纹理 的不同过滤模式"
        item6[ITEM_SUBTITLE] = "本课程将介绍OpenGL ES中可用的不同纹理过滤模式。"
        data.add(item6)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_SIX)

        //////////////////////////////////// 两个异类  ////////////////////////////////////
        // todo  还没有改造完的.... 两个
        val item7: MutableMap<String, Any> = HashMap()
        item7[ITEM_IMAGE] = R.mipmap.ic_lesson_seven
        item7[ITEM_TITLE] = "使用 顶点缓冲区对象（VBO）来绘制多个立方体 "
        item7[ITEM_SUBTITLE] = "学到的知识点：立方体的数量可以改变，可以切换是否使用VBOs,Native可以调用Java方法"
        data.add(item7)
        // 这个i不要递增
        typeMapping.put(i, IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_SEVEN)
        activityMapping.put(i, LessonSevenActivity::class.java)
        // i 递增
        i++

        val item8: MutableMap<String, Any> = HashMap()
        item8[ITEM_IMAGE] = R.mipmap.ic_lesson_eight
        item8[ITEM_TITLE] = "使用 IBOs 来绘制 "
        item8[ITEM_SUBTITLE] = "本课程将介绍索引缓冲区对象（IBO）"
        data.add(item8)
        // 这个i不要递增
        typeMapping.put(i, IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_EIGHT)
        activityMapping.put(i, LessonEightActivity::class.java)
        // i 递增
        i++
        //////////////////////////////////// 两个异类  ////////////////////////////////////

        val itemTextureMap: MutableMap<String, Any> = HashMap()
        itemTextureMap[ITEM_IMAGE] = R.mipmap.ic_texture_map
        itemTextureMap[ITEM_TITLE] = "展示 纹理映射"
        itemTextureMap[ITEM_SUBTITLE] = "纹理映射就是通过为图元的顶点坐标指定恰当的纹理坐标，通过纹理坐标在纹理图中选定特定的纹理区域，最后通过纹理坐标与顶点的映射关系，将选定的纹理区域映射到指定图元上。"
        data.add(itemTextureMap)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TEXTURE_MAP)

        val itemYUVRender: MutableMap<String, Any> = HashMap()
        itemYUVRender[ITEM_IMAGE] = R.mipmap.ic_yuv
        itemYUVRender[ITEM_TITLE] = "YUV 渲染"
        itemYUVRender[ITEM_SUBTITLE] = "YUV 图不能直接用于显示，需要转换为 RGB 格式，而 YUV 转 RGB 是一个逐像素处理的耗时操作，在 CPU 端进行转换效率过低，这时正好可以利用 GPU 强大的并行处理能力来实现 YUV 到 RGB 的转换。"
        data.add(itemYUVRender)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_YUV_RENDER)

        val itemFBO: MutableMap<String, Any> = HashMap()
        itemFBO[ITEM_IMAGE] = R.mipmap.ic_fbo
        itemFBO[ITEM_TITLE] = "使用 FBO 技术，实现将彩色照片渲染成黑白照片"
        itemFBO[ITEM_SUBTITLE] = "学习 FBO(帧缓冲区对象) 离屏渲染"
        data.add(itemFBO)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_FBO)

        //////////////////////////////////// 1个异类  ////////////////////////////////////
        val itemEGL: MutableMap<String, Any> = HashMap()
        itemEGL[ITEM_IMAGE] = R.mipmap.ic_texture_map
        itemEGL[ITEM_TITLE] = "使用 EGL渲染 来实现 马赛克、网格、旋转、边缘、放大、形变等效果"
        itemEGL[ITEM_SUBTITLE] = "学习 使用EGL渲染 技术"
        data.add(itemEGL)
        // 这个i不要递增
        typeMapping.put(i, IMyNativeRendererType.SAMPLE_TYPE_KEY_EGL)
        activityMapping.put(i, EGLActivity::class.java)
        // i 递增
        i++
        //////////////////////////////////// 1个异类  ////////////////////////////////////

        val itemFBOLeg: MutableMap<String, Any> = HashMap()
        itemFBOLeg[ITEM_IMAGE] = R.mipmap.ic_fbo
        itemFBOLeg[ITEM_TITLE] = "FBO 实现 大长腿"
        itemFBOLeg[ITEM_SUBTITLE] = "FBO LEG"
        data.add(itemFBOLeg)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_FBO_LEG)

        val itemCoordSystem: MutableMap<String, Any> = HashMap()
        itemCoordSystem[ITEM_IMAGE] = R.mipmap.ic_texture_map
        itemCoordSystem[ITEM_TITLE] = "对纹理进行缩放和旋转"
        itemCoordSystem[ITEM_SUBTITLE] = "单指可以对图片进行旋转，双指可以进行缩放"
        data.add(itemCoordSystem)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_COORD_SYSTEM)

        val itemBasicLight: MutableMap<String, Any> = HashMap()
        itemBasicLight[ITEM_IMAGE] = R.mipmap.ic_basic_lightings
        itemBasicLight[ITEM_TITLE] = "学习光照基础"
        itemBasicLight[ITEM_SUBTITLE] = "学习冯氏光照模型(Phong Lighting Model)，它由三种元素光组成，分别是环境光(Ambient Lighting)、散射光(Diffuse Lighting)及镜面光(Specular Lighting)。"
        data.add(itemBasicLight)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_BASE_LIGHT)

        val itemMultiLight: MutableMap<String, Any> = HashMap()
        itemMultiLight[ITEM_IMAGE] = R.mipmap.ic_multi_lightings
        itemMultiLight[ITEM_TITLE] = "学习多光照实例"
        itemMultiLight[ITEM_SUBTITLE] = "学习冯氏光照模型(Phong Lighting Model)的多光照实例"
        data.add(itemMultiLight)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_MULTI_LIGHT)

        val itemInstancing: MutableMap<String, Any> = HashMap()
        itemInstancing[ITEM_IMAGE] = R.mipmap.ic_multi_lightings
        itemInstancing[ITEM_TITLE] = "学习 OpenGL ES 实例化（Instancing）"
        itemInstancing[ITEM_SUBTITLE] = "OpenGL ES 实例化（Instancing）是一种只调用一次渲染函数就能绘制出很多物体的技术，可以实现将数据一次性发送给 GPU ，告诉 OpenGL ES 使用一个绘制函数，将这些数据绘制成多个物体。"
        data.add(itemInstancing)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_INSTANCING)

        val itemStencilTesting: MutableMap<String, Any> = HashMap()
        itemStencilTesting[ITEM_IMAGE] = R.mipmap.ic_stencil_testing
        itemStencilTesting[ITEM_TITLE] = "学习 OpenGL ES 模板测试"
        itemStencilTesting[ITEM_SUBTITLE] = "OpenGL ES 模板测试与深度测试类似，主要作用是利用模板缓冲区(Stencil Buffer)所保存的模板值决定当前片段是否被丢弃，且发生于深度测试之前。"
        data.add(itemStencilTesting)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_STENCIL_TESTING)

        val itemBLENDING: MutableMap<String, Any> = HashMap()
        itemBLENDING[ITEM_IMAGE] = R.mipmap.ic_stencil_testing
        itemBLENDING[ITEM_TITLE] = "学习 OpenGL ES 混合"
        itemBLENDING[ITEM_SUBTITLE] = "OpenGL ES 混合本质上是将 2 个片元的颜色进行调和，产生一个新的颜色。OpenGL ES 混合发生在片元通过各项测试之后，准备进入帧缓冲区的片元和原有的片元按照特定比例加权计算出最终片元的颜色值，不再是新（源）片元直接覆盖缓冲区中的（目标）片元。"
        data.add(itemBLENDING)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_BLENDING)


        val itemParticleSystem2: MutableMap<String, Any?> = HashMap()
        itemParticleSystem2[ITEM_IMAGE] = R.mipmap.ic_particles2
        itemParticleSystem2[ITEM_TITLE] = "展示一个不停变化位置的粒子效果"
        itemParticleSystem2[ITEM_SUBTITLE] = "再次实现一个粒子效果，不一样的实现方式"
        data.add(itemParticleSystem2)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM2)

        val itemSkybox: MutableMap<String, Any?> = HashMap()
        itemSkybox[ITEM_IMAGE] = R.mipmap.ic_skybox
        itemSkybox[ITEM_TITLE] = "展示一个立方体贴图（天空盒）,可以旋转"
        itemSkybox[ITEM_SUBTITLE] = "使用立方体贴图实现一个天空盒效果"
        data.add(itemSkybox)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SKYBOX)

        val itemPBO: MutableMap<String, Any?> = HashMap()
        itemPBO[ITEM_IMAGE] = R.mipmap.ic_pbo
        itemPBO[ITEM_TITLE] = "展示一个PBO示例"
        itemPBO[ITEM_SUBTITLE] = "OpenGL PBO（Pixel Buffer Object），被称为像素缓冲区对象，主要被用于异步像素传输操作。PBO 仅用于执行像素传输，不连接到纹理，且与 FBO （帧缓冲区对象）无关。"
        data.add(itemPBO)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_PBO)

        val itemBeatingHeart: MutableMap<String, Any?> = HashMap()
        itemBeatingHeart[ITEM_IMAGE] = R.mipmap.ic_beatingheart
        itemBeatingHeart[ITEM_TITLE] = "展示一段 ShaderToy的特效：跳动的心"
        itemBeatingHeart[ITEM_SUBTITLE] = "一个ShaderToy上的案例，移植到Android端来展示"
        data.add(itemBeatingHeart)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_BEATING_HEART)

        val itemCloud: MutableMap<String, Any?> = HashMap()
        itemCloud[ITEM_IMAGE] = R.mipmap.ic_cloud
        itemCloud[ITEM_TITLE] = "展示一段 ShaderToy的特效：不断漂浮的云朵"
        itemCloud[ITEM_SUBTITLE] = "一个ShaderToy上的案例，移植到Android端来展示"
        data.add(itemCloud)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_CLOUD)

        val itemTimeTunnel: MutableMap<String, Any?> = HashMap()
        itemTimeTunnel[ITEM_IMAGE] = R.mipmap.ic_time_tunnel
        itemTimeTunnel[ITEM_TITLE] = "展示一段 ShaderToy的特效：时光隧道"
        itemTimeTunnel[ITEM_SUBTITLE] = "一个ShaderToy上的案例，移植到Android端来展示"
        data.add(itemTimeTunnel)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_TIME_TUNNEL)

        val itemMainSequenceStar: MutableMap<String, Any?> = HashMap()
        itemMainSequenceStar[ITEM_IMAGE] = R.mipmap.ic_main_sequence_start
        itemMainSequenceStar[ITEM_TITLE] = "展示一段 ShaderToy的特效：燃烧的旋转的主序星"
        itemMainSequenceStar[ITEM_SUBTITLE] = "一个ShaderToy上的案例，移植到Android端来展示"
        data.add(itemMainSequenceStar)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_MAIN_SEQUENCE_STAR)

        val itemSkyPath: MutableMap<String, Any?> = HashMap()
        itemSkyPath[ITEM_IMAGE] = R.mipmap.ic_sky_path
        itemSkyPath[ITEM_TITLE] = "展示一段 ShaderToy的特效：天路"
        itemSkyPath[ITEM_SUBTITLE] = "一个ShaderToy上的案例，移植到Android端来展示"
        data.add(itemSkyPath)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_SKY_PATH)

        val itemADay: MutableMap<String, Any?> = HashMap()
        itemADay[ITEM_IMAGE] = R.mipmap.ic_a_day
        itemADay[ITEM_TITLE] = "展示一段 ShaderToy的特效：A Day"
        itemADay[ITEM_SUBTITLE] = "一个ShaderToy上的案例，移植到Android端来展示"
        data.add(itemADay)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_A_DAY)

        val itemAtmosphereSystemTest: MutableMap<String, Any?> = HashMap()
        itemAtmosphereSystemTest[ITEM_IMAGE] = R.mipmap.ic_atmosphere_system_test
        itemAtmosphereSystemTest[ITEM_TITLE] = "展示一段 ShaderToy的特效：Atmosphere system test "
        itemAtmosphereSystemTest[ITEM_SUBTITLE] = "一个ShaderToy上的案例，移植到Android端来展示"
        data.add(itemAtmosphereSystemTest)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_ATMOSPHERE_SYSTEM_TEST)

        val itemBezierCurve: MutableMap<String, Any?> = HashMap()
        itemBezierCurve[ITEM_IMAGE] = R.mipmap.ic_bezier_currve
        itemBezierCurve[ITEM_TITLE] = "展示几段 贝塞尔曲线 绘制的曲边扇形 混合而成的效果 "
        itemBezierCurve[ITEM_SUBTITLE] = "学习如何绘制一段 贝塞尔曲线"
        data.add(itemBezierCurve)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_BEZIER_CURVE)

        val itemBigEyes: MutableMap<String, Any?> = HashMap()
        itemBigEyes[ITEM_IMAGE] = R.mipmap.ic_big_eyes
        itemBigEyes[ITEM_TITLE] = "展示 一个随着时间变化逐渐放大眼睛的效果 "
        itemBigEyes[ITEM_SUBTITLE] = "学习如何实现 大眼效果"
        data.add(itemBigEyes)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_BIG_EYES)

        val itemFaceSlender: MutableMap<String, Any?> = HashMap()
        itemFaceSlender[ITEM_IMAGE] = R.mipmap.ic_face_slender
        itemFaceSlender[ITEM_TITLE] = "展示 一个随着时间变化逐渐瘦脸的效果 "
        itemFaceSlender[ITEM_SUBTITLE] = "学习如何实现 瘦脸效果"
        data.add(itemFaceSlender)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_FACE_SLENDER)

        val itemBigHead: MutableMap<String, Any?> = HashMap()
        itemBigHead[ITEM_IMAGE] = R.mipmap.ic_big_head
        itemBigHead[ITEM_TITLE] = "展示 一个随着时间变化头部逐渐放大的效果 "
        itemBigHead[ITEM_SUBTITLE] = "学习如何实现 大头效果"
        data.add(itemBigHead)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_BIG_HEAD)

        val itemRataryHead: MutableMap<String, Any?> = HashMap()
        itemRataryHead[ITEM_IMAGE] = R.mipmap.ic_big_head
        itemRataryHead[ITEM_TITLE] = "展示 一个随着时间变化头部跟着晃动的效果 "
        itemRataryHead[ITEM_SUBTITLE] = "学习如何实现 头部晃动效果"
        data.add(itemRataryHead)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_RATARY_HEAD)

        val itemVisualizeAudio: MutableMap<String, Any?> = HashMap()
        itemVisualizeAudio[ITEM_IMAGE] = R.mipmap.ic_visual_audio
        itemVisualizeAudio[ITEM_TITLE] = "展示 一个 可视化实时音频 的效果 "
        itemVisualizeAudio[ITEM_SUBTITLE] = "学习如何实现 可视化实时音频 效果"
        data.add(itemVisualizeAudio)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_VISUALIZE_AUDIO)
    }

    companion object {
        const val ITEM_IMAGE = "item_image"
        const val ITEM_TITLE = "item_title"
        const val ITEM_SUBTITLE = "item_subtitle"
    }
}