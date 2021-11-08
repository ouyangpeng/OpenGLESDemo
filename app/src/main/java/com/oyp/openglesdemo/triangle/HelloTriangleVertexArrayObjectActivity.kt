package com.oyp.openglesdemo.triangle

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity

class HelloTriangleVertexArrayObjectActivity : BaseGLActivity() {
    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return HelloTriangleVertexArrayObjectNativeRenderer()
    }
}