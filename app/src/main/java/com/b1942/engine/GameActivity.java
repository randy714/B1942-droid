package com.b1942.engine;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.Toast;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GameActivity extends Activity implements GLSurfaceView.Renderer {

    private GLSurfaceView glView;
    private TouchControlsView controlsView;
    private String gamePath;
    private float lastTouchX, lastTouchY;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Ambil path lokasi game yang telah dipilih oleh user
        SharedPreferences prefs = getSharedPreferences("B1942_Prefs", Context.MODE_PRIVATE);
        gamePath = getIntent().getStringExtra("GAME_PATH");
        if (gamePath == null || gamePath.isEmpty()) {
            gamePath = prefs.getString("custom_game_path", EngineExtractor.getGameDir());
        }

        FrameLayout container = new FrameLayout(this);

        // Native Surface OpenGL Viewport
        glView = new GLSurfaceView(this);
        glView.setEGLContextClientVersion(2);
        glView.setRenderer(this);
        glView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        container.addView(glView);

        // Overlay Touch Controls
        controlsView = new TouchControlsView(this);
        container.addView(controlsView);

        setContentView(container);
        setupTouchLookHandler();
    }

    private void setupTouchLookHandler() {
        controlsView.setOnTouchListener((v, event) -> {
            int action = event.getActionMasked();
            float x = event.getX();
            float y = event.getY();

            switch (action) {
                case MotionEvent.ACTION_DOWN:
                case MotionEvent.ACTION_POINTER_DOWN:
                    lastTouchX = x;
                    lastTouchY = y;
                    break;
                case MotionEvent.ACTION_MOVE:
                    float dx = x - lastTouchX;
                    float dy = y - lastTouchY;
                    NativeEngine.nativeTouchEvent(dx, dy);
                    lastTouchX = x;
                    lastTouchY = y;
                    break;
            }
            return false;
        });
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {}

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        boolean initialized = NativeEngine.nativeInit(gamePath, width, height);
        if (!initialized) {
            runOnUiThread(() -> Toast.makeText(GameActivity.this, 
                "Warning: Could not open game archives at " + gamePath, Toast.LENGTH_LONG).show());
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        NativeEngine.nativeRender();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (glView != null) glView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (glView != null) glView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        NativeEngine.nativeShutdown();
    }
    }
