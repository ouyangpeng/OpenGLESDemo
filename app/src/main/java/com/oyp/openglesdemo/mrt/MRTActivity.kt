package com.oyp.openglesdemo.mrt

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity
import com.oyp.openglesdemo.texture.Mipmap2DRenderer

class MRTActivity : BaseGLActivity() {

    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return MRTRenderer()
    }
}