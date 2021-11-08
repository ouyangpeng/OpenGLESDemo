package com.oyp.openglesdemo.lesson.lesson6

internal interface Action {
    fun setMinFilter(filter: Int)
    fun setMagFilter(filter: Int)
    fun setDelta(deltaX: Float, deltaY: Float)
}