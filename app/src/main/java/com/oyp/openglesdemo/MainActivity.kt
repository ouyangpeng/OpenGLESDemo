package com.oyp.openglesdemo

import android.app.Activity
import android.app.ListActivity
import android.content.Intent
import android.os.Bundle
import android.util.SparseArray
import android.widget.AdapterView.OnItemClickListener
import android.widget.SimpleAdapter
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
        item0[ITEM_TITLE] =
            getText(R.string.lesson_triangle)
        item0[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(item0)
        activityMapping.put(i++, HelloTriangleActivity::class.java)

        val itemHelloTriangle2: MutableMap<String, Any?> = HashMap()
        itemHelloTriangle2[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangle2[ITEM_TITLE] =
            getText(R.string.lesson_triangle)
        itemHelloTriangle2[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangle2)
        activityMapping.put(i++, HelloTriangle2Activity::class.java)

        val itemHelloTriangle3: MutableMap<String, Any?> = HashMap()
        itemHelloTriangle3[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangle3[ITEM_TITLE] =
            getText(R.string.lesson_triangle)
        itemHelloTriangle3[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangle3)
        activityMapping.put(i++, HelloTriangle3Activity::class.java)

        val itemHelloTriangleMapBuffers: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleMapBuffers[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangleMapBuffers[ITEM_TITLE] =
            getText(R.string.lesson_triangle)
        itemHelloTriangleMapBuffers[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangleMapBuffers)
        activityMapping.put(i++, HelloTriangleMapBuffersActivity::class.java)


        val itemHelloTriangleVertexArrayObject: MutableMap<String, Any?> = HashMap()
        itemHelloTriangleVertexArrayObject[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        itemHelloTriangleVertexArrayObject[ITEM_TITLE] =
            getText(R.string.lesson_triangle)
        itemHelloTriangleVertexArrayObject[ITEM_SUBTITLE] =
            getText(R.string.lesson_triangle_subtitle)
        data.add(itemHelloTriangleVertexArrayObject)
        activityMapping.put(i++, HelloTriangleVertexArrayObjectActivity::class.java)


        val item1: MutableMap<String, Any?> = HashMap()
        item1[ITEM_IMAGE] = R.mipmap.ic_lesson_one
        item1[ITEM_TITLE] =
            getText(R.string.lesson_one)
        item1[ITEM_SUBTITLE] =
            getText(R.string.lesson_one_subtitle)
        data.add(item1)
        activityMapping.put(i++, LessonOneActivity::class.java)

//        val item2: MutableMap<String, Any?> = HashMap()
//        item2[ITEM_IMAGE] =  R.mipmap.ic_lesson_two
//        item2[ITEM_TITLE] =
//            getText(R.string.lesson_two)
//        item2[ITEM_SUBTITLE] =
//            getText(R.string.lesson_two_subtitle)
//        data.add(item2)
//        activityMapping.put(i++, LessonTwoActivity::class.java)
//
//        val item3: MutableMap<String, Any?> = HashMap()
//        item3[ITEM_IMAGE] =  R.mipmap.ic_lesson_three
//        item3[ITEM_TITLE] =
//            getText(R.string.lesson_three)
//        item3[ITEM_SUBTITLE] =
//            getText(R.string.lesson_three_subtitle)
//        data.add(item3)
//        activityMapping.put(i++, LessonThreeActivity::class.java)
//
//        val item4: MutableMap<String, Any?> = HashMap()
//        item4[ITEM_IMAGE] =  R.mipmap.ic_lesson_four
//        item4[ITEM_TITLE] =
//            getText(R.string.lesson_four)
//        item4[ITEM_SUBTITLE] =
//            getText(R.string.lesson_four_subtitle)
//        data.add(item4)
//        activityMapping.put(i++, LessonFourActivity::class.java)
//
//        val item5: MutableMap<String, Any?> = HashMap()
//        item5[ITEM_IMAGE] =  R.mipmap.ic_lesson_five
//        item5[ITEM_TITLE] =
//            getText(R.string.lesson_five)
//        item5[ITEM_SUBTITLE] =
//            getText(R.string.lesson_five_subtitle)
//        data.add(item5)
//        activityMapping.put(i++, LessonFiveActivity::class.java)
//
//        val item6: MutableMap<String, Any?> = HashMap()
//        item6[ITEM_IMAGE] =  R.mipmap.ic_lesson_six
//        item6[ITEM_TITLE] =
//            getText(R.string.lesson_six)
//        item6[ITEM_SUBTITLE] =
//            getText(R.string.lesson_six_subtitle)
//        data.add(item6)
//        activityMapping.put(i++, LessonSixActivity::class.java)
//
//        val item7: MutableMap<String, Any?> = HashMap()
//        item7[ITEM_IMAGE] =  R.mipmap.ic_lesson_seven
//        item7[ITEM_TITLE] =
//            getText(R.string.lesson_seven)
//        item7[ITEM_SUBTITLE] =
//            getText(R.string.lesson_seven_subtitle)
//        data.add(item7)
//        activityMapping.put(i++, LessonSevenActivity::class.java)
//
//        val item8: MutableMap<String, Any?> = HashMap()
//        item8[ITEM_IMAGE] =  R.mipmap.ic_lesson_eight
//        item8[ITEM_TITLE] =
//            getText(R.string.lesson_eight)
//        item8[ITEM_SUBTITLE] =
//            getText(R.string.lesson_eight_subtitle)
//        data.add(item8)
//        activityMapping.put(i++, LessonEightActivity::class.java)

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