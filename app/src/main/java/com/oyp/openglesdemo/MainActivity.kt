package com.oyp.openglesdemo

import android.app.Activity
import android.app.ListActivity
import android.content.Intent
import android.os.Bundle
import android.util.SparseArray
import android.widget.AdapterView.OnItemClickListener
import android.widget.SimpleAdapter
import com.oyp.openglesdemo.cube.HelloCubeSimpleVertexShaderActivity
import com.oyp.openglesdemo.lesson1.LessonOneActivity
import com.oyp.openglesdemo.triangle.*
import java.util.*

class MainActivity : ListActivity() {

    private val ITEM_IMAGE = "item_image"
    private val ITEM_TITLE = "item_title"
    private val ITEM_SUBTITLE = "item_subtitle"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Initialize data
        val data: MutableList<Map<String, Any?>> = ArrayList()
        val activityMapping = SparseArray<Class<out Activity?>>()

        var i = 0

        val item0: MutableMap<String, Any?> = HashMap()
        item0[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        item0[ITEM_TITLE] = " Hello Triangle "
        item0[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(item0)
        activityMapping.put(i++, HelloTriangleActivity::class.java)

        val itemHelloTriangle2: MutableMap<String, Any?> = HashMap()
        itemHelloTriangle2[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangle2[ITEM_TITLE] =" Hello Triangle 2"
        itemHelloTriangle2[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangle2)
        activityMapping.put(i++, HelloTriangle2Activity::class.java)

        val itemHelloTriangle3: MutableMap<String, Any?> = HashMap()
        itemHelloTriangle3[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangle3[ITEM_TITLE] = " Hello Triangle 3"
        itemHelloTriangle3[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangle3)
        activityMapping.put(i++, HelloTriangle3Activity::class.java)

        val itemHelloTriangleMapBuffers: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleMapBuffers[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangleMapBuffers[ITEM_TITLE] = " Hello Triangle MapBuffers"
        itemHelloTriangleMapBuffers[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangleMapBuffers)
        activityMapping.put(i++, HelloTriangleMapBuffersActivity::class.java)


        val itemHelloTriangleVertexArrayObject: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleVertexArrayObject[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangleVertexArrayObject[ITEM_TITLE] = " Hello Triangle VertexArrayObject"
        itemHelloTriangleVertexArrayObject[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangleVertexArrayObject)
        activityMapping.put(i++, HelloTriangleVertexArrayObjectActivity::class.java)

        val itemHelloTriangleVertexBufferObject: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleVertexBufferObject[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangleVertexBufferObject[ITEM_TITLE] = " Hello Triangle VertexBufferObject"
        itemHelloTriangleVertexBufferObject[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangleVertexBufferObject)
        activityMapping.put(i++, HelloTriangleVertexBufferObjectActivity::class.java)


        val itemHelloTriangleSimpleVertexShader: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleSimpleVertexShader[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangleSimpleVertexShader[ITEM_TITLE] = " Hello Cube SimpleVertexShader"
        itemHelloTriangleSimpleVertexShader[ITEM_SUBTITLE] =  " Show a cube translate"
        data.add(itemHelloTriangleSimpleVertexShader)
        activityMapping.put(i++, HelloCubeSimpleVertexShaderActivity::class.java)


        val item1: MutableMap<String, Any?> = HashMap()
        item1[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        item1[ITEM_TITLE] =
            getText(R.string.lesson_one)
        item1[ITEM_SUBTITLE] =
            getText(R.string.lesson_one_subtitle)
        data.add(item1)
        activityMapping.put(i++, LessonOneActivity::class.java)

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
            val activityToLaunch = activityMapping[position]
            if (activityToLaunch != null) {
                val launchIntent = Intent(this@MainActivity, activityToLaunch)
                startActivity(launchIntent)
            }
        }
    }
}