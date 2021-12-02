package com.oyp.openglesdemo.adapter

import android.content.Context
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import android.widget.TextView
import android.widget.ImageView
import android.view.LayoutInflater
import com.oyp.openglesdemo.R
import com.oyp.openglesdemo.activity.MainActivity


class GLRecyclerAdapter(
    private var mData: MutableList<Map<String, Any?>>,
    private var mContext: Context
) : RecyclerView.Adapter<GLRecyclerAdapter.GLViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): GLViewHolder {
        val itemView: View =
            LayoutInflater.from(mContext).inflate(R.layout.toc_item, parent, false)
        return GLViewHolder(itemView)
    }

    override fun onBindViewHolder(holder: GLViewHolder, position: Int) {
        mData[position].let {
            holder.mImage?.setImageResource(it.getValue(MainActivity.ITEM_IMAGE) as Int)
            holder.mTitle?.text = it.getValue(MainActivity.ITEM_TITLE) as String
            holder.mSubTitle?.text = it.getValue(MainActivity.ITEM_SUBTITLE) as String
        }

        // 设置点击事件
        holder.itemView.setOnClickListener {
            listener?.invoke(position)
        }
    }

    override fun getItemCount(): Int {
        return mData.size
    }

    inner class GLViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        var mImage: ImageView? = null
        var mTitle: TextView? = null
        var mSubTitle: TextView? = null

        init {
            mImage = itemView.findViewById<View>(R.id.gl_Image) as ImageView
            mTitle = itemView.findViewById<View>(R.id.gl_Title) as TextView
            mSubTitle = itemView.findViewById<View>(R.id.gl_SubTitle) as TextView
        }
    }

    //第一步 定义接口
    private var listener: ((position: Int) -> Unit)? = null

    //第二步， 写一个公共的方法
    fun setOnItemClickListener(listener: (position: Int) -> Unit) {
        this.listener = listener
    }
}