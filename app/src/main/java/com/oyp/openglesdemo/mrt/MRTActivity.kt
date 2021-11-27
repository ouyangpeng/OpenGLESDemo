package com.oyp.openglesdemo.mrt

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity

class MRTActivity : BaseGLActivity() {

    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return MRTRenderer()
    }
}