package com.b1942.engine;

import android.app.Activity;
import android.graphics.Color;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GameActivity extends Activity {

    static {
        System.loadLibrary("b1942_engine");
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

        applyFullscreenSticky();

        FrameLayout rootLayout = new FrameLayout(this);
        rootLayout.setBackgroundColor(Color.parseColor("#0D0B0A")); // Dark Espresso

        // Setup OpenGL Surface
        glView = new GLSurfaceView(this);
        glView.setEGLContextClientVersion(3);
        glView.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                nativeGLInit();
            }

            @Override
            public void onSurfaceChanged(GL10 gl, int width, int height) {
                nativeGLResize(width, height);
            }

            @Override
            public void onDrawFrame(GL10 gl) {
                nativeGLRender();
            }
        });
        rootLayout.addView(glView);

        // Controls Overlay
        RelativeLayout controlsOverlay = createTouchControlsOverlay();
        rootLayout.addView(controlsOverlay);

        setContentView(rootLayout);

        // Native Initializations
        String gamePath = "/sdcard/B1942-droid/game/";
        nativeInit(gamePath);
        nativePlayBink(gamePath + "Movies/Intro.bik");

        // Any screen tap skips intro
        rootLayout.setOnTouchListener((v, event) -> {
            if (isIntroPlaying && event.getAction() == MotionEvent.ACTION_DOWN) {
                skipIntro();
                return true;
            }
            return false;
        });
    }

    private void skipIntro() {
        if (isIntroPlaying) {
            isIntroPlaying = false;
            nativeStopBink();
            nativeGameControl(1, true); // Trigger FIRE action to skip
        }
    }

    private RelativeLayout createTouchControlsOverlay() {
        RelativeLayout layout = new RelativeLayout(this);

        // 10 Button definitions: {Name, ID, Color, Width, Height, AlignRules...}
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

    private void addButton(RelativeLayout parent, String name, int actionId, String colorHex, int w, int h, int rule1, int rule2, int margin1, int margin2) {
        Button btn = new Button(this);
        btn.setText(name);
        btn.setTextColor(Color.parseColor("#E0A96D")); // Gold text
        btn.setBackgroundColor(Color.parseColor(colorHex));
        btn.setTextSize(10f);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(w, h);
        params.addRule(rule1);
        params.addRule(rule2);

        if (rule1 == RelativeLayout.ALIGN_PARENT_BOTTOM) params.bottomMargin = margin2;
        if (rule1 == RelativeLayout.ALIGN_PARENT_TOP) params.topMargin = margin2;
        if (rule2 == RelativeLayout.ALIGN_PARENT_LEFT) params.leftMargin = margin1;
        if (rule2 == RelativeLayout.ALIGN_PARENT_RIGHT) params.rightMargin = margin1;

        btn.setLayoutParams(params);

        btn.setOnTouchListener((v, event) -> {
            if (isIntroPlaying) {
                skipIntro();
            }
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                nativeGameControl(actionId, true);
            } else if (event.getAction() == MotionEvent.ACTION_UP || event.getAction() == MotionEvent.ACTION_CANCEL) {
                nativeGameControl(actionId, false);
            }
            return true;
        });

        parent.addView(btn);
    }

    private void applyFullscreenSticky() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            getWindow().setDecorFitsSystemWindows(false);
            WindowInsetsController controller = getWindow().getInsetsController();
            if (controller != null) {
                controller.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else {
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            );
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (glView != null) glView.onResume();
        applyFullscreenSticky();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (glView != null) glView.onPause();
    }
  }
