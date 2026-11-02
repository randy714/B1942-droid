package com.b1942.engine;

public class NativeEngine {
    static {
        System.loadLibrary("b1942_engine");
    }

    public static native boolean nativeInit(String gamePath, int width, int height);
    public static native void nativeResize(int width, int height);
    public static native void nativeRender();
    public static native void nativeTouchEvent(float dx, float dy);
    public static native void nativeGameControl(int buttonId, boolean state);
    public static native void nativeShutdown();
}
