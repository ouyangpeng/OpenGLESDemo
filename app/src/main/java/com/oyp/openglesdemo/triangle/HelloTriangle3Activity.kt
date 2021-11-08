package com.oyp.openglesdemo.triangle

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity

class HelloTriangle3Activity : BaseGLActivity() {
    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return HelloTriangle3NativeRenderer()
    }
}