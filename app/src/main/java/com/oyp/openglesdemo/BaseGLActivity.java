package com.oyp.openglesdemo;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.Nullable;

public abstract class BaseGLActivity extends Activity {
    /**
     * Hold a reference to our GLSurfaceView
     */
    protected GLSurfaceView mGLSurfaceView;

    protected GLSurfaceView.Renderer renderer;

    protected abstract GLSurfaceView.Renderer getRender();

    protected abstract GLSurfaceView getSurfaceView();

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        renderer = getRender();
        mGLSurfaceView = getSurfaceView();

        if(detectOpenGLES30()){
            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.
            int CONTEXT_CLIENT_VERSION = 3;
            mGLSurfaceView.setEGLContextClientVersion(CONTEXT_CLIENT_VERSION);
            mGLSurfaceView.setRenderer(renderer);
        } else {
            Log.e("HelloTriangle", "OpenGL ES 3.0 not supported on device.  Exiting...");
            return;
        }
        setContentView(mGLSurfaceView);
    }

    private Boolean detectOpenGLES30(){
        ActivityManager am = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return info.reqGlEsVersion >= 0x30000;
    }

    @Override
    protected void onResume() {
        // The activity must call the GL surface view's onResume() on activity onResume().
        super.onResume();
        mGLSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        // The activity must call the GL surface view's onPause() on activity onPause().
        super.onPause();
        mGLSurfaceView.onPause();
    }
}
