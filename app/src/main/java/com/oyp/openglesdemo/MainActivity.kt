package com.oyp.openglesdemo

import android.app.ListActivity
import android.content.Intent
import android.os.Bundle
import android.util.SparseArray
import android.widget.AdapterView.OnItemClickListener
import android.widget.SimpleAdapter
import com.oyp.openglesdemo.lesson.*
import java.util.*

class MainActivity : ListActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val (data: MutableList<Map<String, Any?>>, typeMapping) = initData()

        val dataAdapter = SimpleAdapter(
            this,
            data,
            R.layout.toc_item,
            arrayOf(
                ITEM_IMAGE,
                ITEM_TITLE,
                ITEM_SUBTITLE
            ),
            intArrayOf(R.id.Image, R.id.Title, R.id.SubTitle)
        )

        setListAdapter(dataAdapter)

        getListView().onItemClickListener = OnItemClickListener { _, _, position, _ ->

            val type = typeMapping[position]
            if (type != 0) {
                val launchIntent = Intent(this@MainActivity, NativeRenderActivity::class.java)
                launchIntent.putExtra(IMyNativeRendererType.RENDER_TYPE, type)
                startActivity(launchIntent)
            }


//            val activityToLaunch = activityMapping[position]
//            if (activityToLaunch != null) {
//                val launchIntent = Intent(this@MainActivity, activityToLaunch)
//                startActivity(launchIntent)
//            }
        }
    }

    private fun initData(): Pair<MutableList<Map<String, Any?>>, SparseArray<Int>> {
        // Initialize data
        val data: MutableList<Map<String, Any?>> = ArrayList()
        val typeMapping = SparseArray<Int>()

        var i = 0

        val item0: MutableMap<String, Any?> = HashMap()
        item0[ITEM_IMAGE] = R.mipmap.ic_trangle
        item0[ITEM_TITLE] = "展示一个基本的红色三角形"
        item0[ITEM_SUBTITLE] = "颜色在片段着色器写死的红色"
        data.add(item0)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_TRIANGLE)

        val itemHelloTriangle2: MutableMap<String, Any?> = HashMap()
        itemHelloTriangle2[ITEM_IMAGE] = R.mipmap.ic_trangle2
        itemHelloTriangle2[ITEM_TITLE] = "展示一个基本的蓝色三角形"
        itemHelloTriangle2[ITEM_SUBTITLE] = "颜色由glVertexAttrib4fv传给片段着色器"
        data.add(itemHelloTriangle2)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_TRIANGLE2)

        val itemHelloTriangle3: MutableMap<String, Any?> = HashMap()
        itemHelloTriangle3[ITEM_IMAGE] = R.mipmap.ic_trangle3
        itemHelloTriangle3[ITEM_TITLE] = "展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形"
        itemHelloTriangle3[ITEM_SUBTITLE] = "使用了顶点缓冲对象(Vertex Buffer Objects, VBO) 和 EBO 技术"
        data.add(itemHelloTriangle3)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_TRIANGLE3)

        val itemHelloTriangleMapBuffers: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleMapBuffers[ITEM_IMAGE] = R.mipmap.ic_trangle_mapbuffer
        itemHelloTriangleMapBuffers[ITEM_TITLE] = "展示一个基本的由红、绿、蓝三种颜色绘制而成的三角形"
        itemHelloTriangleMapBuffers[ITEM_SUBTITLE] =
            "使用了顶点缓冲对象(Vertex Buffer Objects, VBO) 、 EBO 和 映射缓冲区对象(Map Buffer) 技术"
        data.add(itemHelloTriangleMapBuffers)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TRIANGLE_MAP_BUFFERS)

        val itemHelloTriangleVertexArrayObject: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleVertexArrayObject[ITEM_IMAGE] = R.mipmap.ic_trangle_mapbuffer
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
        itemTextureWrap[ITEM_SUBTITLE] =  "学习GL_REPEAT、GL_CLAMP_TO_EDGE、GL_MIRRORED_REPEAT三种不同纹理包装模式"
        data.add(itemTextureWrap)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TEXTURE_WRAP)


        val itemMultiTexture: MutableMap<String, Any?> = HashMap()
        itemMultiTexture[ITEM_IMAGE] = R.mipmap.ic_multi_texture
        itemMultiTexture[ITEM_TITLE] = "展示一个多重纹理"
        itemMultiTexture[ITEM_SUBTITLE] =  "学习如何使用多重纹理"
        data.add(itemMultiTexture)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_MULTI_TEXTURE)


        val itemParticleSystem: MutableMap<String, Any?> = HashMap()
        itemParticleSystem[ITEM_IMAGE] = R.mipmap.ic_particle_system
        itemParticleSystem[ITEM_TITLE] = "展示一个不停变化位置的爆炸效果"
        itemParticleSystem[ITEM_SUBTITLE] =  "学习如何用点精灵渲染粒子爆炸效果"
        data.add(itemParticleSystem)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM)


        val itemParticleSystemTransformFeed: MutableMap<String, Any?> = HashMap()
        itemParticleSystemTransformFeed[ITEM_IMAGE] = R.mipmap.ic_particle_system_transform_feedback
        itemParticleSystemTransformFeed[ITEM_TITLE] = "使用变化反馈的粒子系统实现源源不断喷射的爆炸效果"
        itemParticleSystemTransformFeed[ITEM_SUBTITLE] =  "使用变化反馈的粒子系统"
        data.add(itemParticleSystemTransformFeed)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM_TRANSFORM_FEEDBACK)

        val itemNoise3D: MutableMap<String, Any?> = HashMap()
        itemNoise3D[ITEM_IMAGE] = R.mipmap.ic_noise3d
        itemNoise3D[ITEM_TITLE] = "展示 3D纹理噪音 "
        itemNoise3D[ITEM_SUBTITLE] =  "学习实现3D纹理噪音"
        data.add(itemNoise3D)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_NOISE3D)

        val itemMRT: MutableMap<String, Any?> = HashMap()
        itemMRT[ITEM_IMAGE] = R.mipmap.ic_mrt
        itemMRT[ITEM_TITLE] = "展示 多重渲染技术同时渲染到4个目标"
        itemMRT[ITEM_SUBTITLE] =  "学习 MRT(多重渲染目标) 技术"
        data.add(itemMRT)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_MRT)

        val itemTerrainRender: MutableMap<String, Any?> = HashMap()
        itemTerrainRender[ITEM_IMAGE] = R.mipmap.ic_terrainrender
        itemTerrainRender[ITEM_TITLE] = "展示一幅立体地形画"
        itemTerrainRender[ITEM_SUBTITLE] =  "学习 用顶点纹理读取渲染地形 技术"
        data.add(itemTerrainRender)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_TERRAIN_RENDER)

        val itemShadows: MutableMap<String, Any?> = HashMap()
        itemShadows[ITEM_IMAGE] = R.mipmap.ic_shadows
        itemShadows[ITEM_TITLE] = "展示 深度纹理的阴影 效果图"
        itemShadows[ITEM_SUBTITLE] =  "使用深度纹理的阴影"
        data.add(itemShadows)
        typeMapping.put(i++, IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADOWS)

        val item1: MutableMap<String, Any?> = HashMap()
        item1[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        item1[ITEM_TITLE] =  "展示 三个彩色三角形 不停旋转的效果"
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
        //
        //        val item6: MutableMap<String, Any> = HashMap()
        //        item6[ITEM_IMAGE] = R.mipmap.ic_lesson_six
        //        item6[ITEM_TITLE] = getText(R.string.lesson_six)
        //        item6[ITEM_SUBTITLE] =
        //            getText(R.string.lesson_six_subtitle)
        //        data.add(item6)
        //        activityMapping.put(i++, LessonSixActivity::class.java)
        //
        //
        //        val item7: MutableMap<String, Any> = HashMap()
        //        item7[ITEM_IMAGE] = R.mipmap.ic_lesson_seven
        //        item7[ITEM_TITLE] = getText(R.string.lesson_seven)
        //        item7[ITEM_SUBTITLE] =
        //            getText(R.string.lesson_seven_subtitle)
        //        data.add(item7)
        //        activityMapping.put(i++, LessonSevenActivity::class.java)
        //
        //
        //        val item8: MutableMap<String, Any> = HashMap()
        //        item8[ITEM_IMAGE] = R.mipmap.ic_lesson_eight
        //        item8[ITEM_TITLE] = getText(R.string.lesson_eight)
        //        item8[ITEM_SUBTITLE] =
        //            getText(R.string.lesson_eight_subtitle)
        //        data.add(item8)
        //        activityMapping.put(i++, LessonEightActivity::class.java)
        return Pair(data, typeMapping)
    }

    companion object {
        private const val ITEM_IMAGE = "item_image"
        private const val ITEM_TITLE = "item_title"
        private const val ITEM_SUBTITLE = "item_subtitle"
    }
}