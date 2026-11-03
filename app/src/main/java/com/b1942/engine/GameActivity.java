package com.b1942.engine;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;
import android.widget.Toast;
import java.io.File;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GameActivity extends Activity {

    static {
        try { System.loadLibrary("b1942_engine"); } catch (Throwable e) {}
    }

    private native void nativeInit(String path);
    private native void nativeGameControl(int actionId, boolean pressed);
    private native void nativePlayBink(String moviePath);
    private native void nativeStopBink();
    private native void nativeGLInit();
    private native void nativeGLResize(int width, int height);
    private native void nativeGLRender();

    private boolean isIntroPlaying = true;
    private GLSurfaceView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        checkStoragePermission();
        applyFullscreenSticky();

        FrameLayout rootLayout = new FrameLayout(this);
        rootLayout.setBackgroundColor(Color.parseColor("#0D0B0A"));

        glView = new GLSurfaceView(this);
        glView.setEGLContextClientVersion(3); // OpenGL ES 3.0 Hardware Acceleration
        glView.setRenderer(new GLSurfaceView.Renderer() {
            public void onSurfaceCreated(GL10 gl, EGLConfig config) { try { nativeGLInit(); } catch(Throwable t){} }
            public void onSurfaceChanged(GL10 gl, int w, int h) { try { nativeGLResize(w, h); } catch(Throwable t){} }
            public void onDrawFrame(GL10 gl) { try { nativeGLRender(); } catch(Throwable t){} }
        });
        rootLayout.addView(glView);

        RelativeLayout controlsOverlay = createTouchControlsOverlay();
        rootLayout.addView(controlsOverlay);
        setContentView(rootLayout);

        File gameFolder = new File(Environment.getExternalStorageDirectory(), "B1942-droid/game");
        if (!gameFolder.exists()) gameFolder.mkdirs();
        
        try {
            nativeInit(gameFolder.getAbsolutePath() + "/");
            nativePlayBink(gameFolder.getAbsolutePath() + "/Movies/Intro.bik");
        } catch (Throwable t) {}

        rootLayout.setOnTouchListener((v, event) -> {
            if (isIntroPlaying && event.getAction() == MotionEvent.ACTION_DOWN) {
                isIntroPlaying = false;
                try { nativeStopBink(); nativeGameControl(1, true); } catch (Throwable t) {}
                return true;
            }
            return false;
        });
    }

    private void checkStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R && !Environment.isExternalStorageManager()) {
            try { startActivity(new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)); } catch (Exception e) {}
        }
    }

    private RelativeLayout createTouchControlsOverlay() {
        RelativeLayout layout = new RelativeLayout(this);
        addButton(layout, "MOVE", 0, "#D48C46", 180, 180, RelativeLayout.ALIGN_PARENT_BOTTOM, RelativeLayout.ALIGN_PARENT_LEFT, 60, 60);
        addButton(layout, "FIRE", 1, "#8B1A1A", 160, 160, RelativeLayout.ALIGN_PARENT_BOTTOM, RelativeLayout.ALIGN_PARENT_RIGHT, 60, 60);
        addButton(layout, "AIM", 2, "#D48C46", 130, 130, RelativeLayout.ALIGN_PARENT_BOTTOM, RelativeLayout.ALIGN_PARENT_RIGHT, 240, 180);
        addButton(layout, "JUMP", 3, "#1E1612", 120, 120, RelativeLayout.ALIGN_PARENT_BOTTOM, RelativeLayout.ALIGN_PARENT_RIGHT, 60, 240);
        addButton(layout, "CROUCH", 4, "#1E1612", 120, 120, RelativeLayout.ALIGN_PARENT_BOTTOM, RelativeLayout.ALIGN_PARENT_RIGHT, 200, 60);
        addButton(layout, "RELOAD", 5, "#1E1612", 110, 110, RelativeLayout.ALIGN_PARENT_BOTTOM, RelativeLayout.ALIGN_PARENT_RIGHT, 380, 180);
        addButton(layout, "WEAPON", 6, "#1E1612", 110, 110, RelativeLayout.ALIGN_PARENT_TOP, RelativeLayout.ALIGN_PARENT_RIGHT, 60, 60);
        addButton(layout, "VEHICLE", 7, "#1E1612", 110, 110, RelativeLayout.ALIGN_PARENT_TOP, RelativeLayout.ALIGN_PARENT_RIGHT, 190, 60);
        addButton(layout, "SCORE", 8, "#1E1612", 100, 100, RelativeLayout.ALIGN_PARENT_TOP, RelativeLayout.ALIGN_PARENT_LEFT, 60, 60);
        addButton(layout, "MENU", 9, "#1E1612", 100, 100, RelativeLayout.ALIGN_PARENT_TOP, RelativeLayout.ALIGN_PARENT_LEFT, 180, 60);
        return layout;
    }

    private void addButton(RelativeLayout p, String name, int id, String color, int w, int h, int r1, int r2, int m1, int m2) {
        Button btn = new Button(this);
        btn.setText(name); btn.setTextColor(Color.parseColor("#E0A96D"));
        btn.setBackgroundColor(Color.parseColor(color)); btn.setTextSize(10f);
        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(w, h);
        params.addRule(r1); params.addRule(r2);
        if (r1 == RelativeLayout.ALIGN_PARENT_BOTTOM) params.bottomMargin = m2;
        if (r1 == RelativeLayout.ALIGN_PARENT_TOP) params.topMargin = m2;
        if (r2 == RelativeLayout.ALIGN_PARENT_LEFT) params.leftMargin = m1;
        if (r2 == RelativeLayout.ALIGN_PARENT_RIGHT) params.rightMargin = m1;
        btn.setLayoutParams(params);
        btn.setOnTouchListener((v, e) -> {
            if (isIntroPlaying) { isIntroPlaying = false; try { nativeStopBink(); } catch (Throwable t) {} }
            try {
                if (e.getAction() == MotionEvent.ACTION_DOWN) nativeGameControl(id, true);
                else if (e.getAction() == MotionEvent.ACTION_UP || e.getAction() == MotionEvent.ACTION_CANCEL) nativeGameControl(id, false);
            } catch (Throwable t) {}
            return true;
        });
        p.addView(btn);
    }

    private void applyFullscreenSticky() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            getWindow().setDecorFitsSystemWindows(false);
            WindowInsetsController c = getWindow().getInsetsController();
            if (c != null) { c.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars()); c.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE); }
        } else { getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION); }
    }

    @Override protected void onResume() { super.onResume(); if (glView != null) glView.onResume(); applyFullscreenSticky(); }
    @Override protected void onPause() { super.onPause(); if (glView != null) glView.onPause(); }
          }
