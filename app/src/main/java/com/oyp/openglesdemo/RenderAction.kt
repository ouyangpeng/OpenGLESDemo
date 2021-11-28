package com.oyp.openglesdemo

interface RenderAction {
    fun switchBlendingMode()

    fun setMinFilter(filter: Int)
    fun setMagFilter(filter: Int)
    fun setDelta(deltaX: Float, deltaY: Float)
}