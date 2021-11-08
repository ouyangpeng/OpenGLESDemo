package com.oyp.openglesdemo.lesson.lesson7

internal interface Action {
    fun init()
    fun destroy()
    fun setDelta(deltaX: Float, deltaY: Float)
    fun decreaseCubeCount()
    fun increaseCubeCount()
    fun toggleVBOs()
    fun toggleStride()
}