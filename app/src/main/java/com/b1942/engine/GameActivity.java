package com.b1942.engine;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;
import android.widget.Toast;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GameActivity extends Activity implements GLSurfaceView.Renderer {

    static {
        // Memuat library engine 30MB Anda
        System.loadLibrary("b1942_engine");
    }

    // Deklarasi fungsi Native C++
    public native void nativeInitEngine(String gamePath);
    public native void nativeUpdateInput(int keyCode, boolean isPressed);
    public native void nativeOnUpdate(float deltaTime);
    public native void nativeOnLook(float dx, float dy, float dt);
    public native void nativeInitGL();
    public native void nativeResizeGL(int w, int h);
    public native void nativeRenderGL();
    public native byte[] nativeGetAssetData(String name); // Mengambil data dari RFA

    private GLSurfaceView glView;
    private long lastFrameTime = System.nanoTime();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Pengaturan Fullscreen & Notch (Layar Poni)
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            getWindow().getAttributes().layoutInDisplayCutoutMode = 
                WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }

        hideSystemUI();
        checkStoragePermission();

        FrameLayout mainLayout = new FrameLayout(this);

        // 1. OpenGL View (Layar Game)
        glView = new GLSurfaceView(this);
        glView.setEGLContextClientVersion(2);
        glView.setRenderer(this);
        mainLayout.addView(glView);

        // 2. Tactical Touch Controls (Espresso UI)
        RelativeLayout controls = new RelativeLayout(this);
        setupTacticalButtons(controls);
        mainLayout.addView(controls);

        setContentView(mainLayout);

        // Inisialisasi Engine ke folder game BF1942 Anda
        String gamePath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/B1942-droid/game";
        nativeInitEngine(gamePath);
    }

    private void setupTacticalButtons(RelativeLayout layout) {
        int espressoCard = Color.parseColor("#1E1612");
        int accentGold = Color.parseColor("#D48C46");
        int fireRed = Color.parseColor("#8B1A1A");

        // SISI KIRI: Movement
        addBtn(layout, "FWD", 1, RelativeLayout.ALIGN_PARENT_LEFT, RelativeLayout.ALIGN_PARENT_TOP, 100, 350, espressoCard, accentGold);
        addBtn(layout, "BACK", 2, RelativeLayout.ALIGN_PARENT_LEFT, RelativeLayout.ALIGN_PARENT_BOTTOM, 100, 50, espressoCard, accentGold);
        addBtn(layout, "LEFT", 3, RelativeLayout.ALIGN_PARENT_LEFT, RelativeLayout.ALIGN_PARENT_BOTTOM, 20, 200, espressoCard, accentGold);
        addBtn(layout, "RIGHT", 4, RelativeLayout.ALIGN_PARENT_LEFT, RelativeLayout.ALIGN_PARENT_BOTTOM, 180, 200, espressoCard, accentGold);

        // SISI KANAN: Action
        addBtn(layout, "FIRE", 7, RelativeLayout.ALIGN_PARENT_RIGHT, RelativeLayout.ALIGN_PARENT_BOTTOM, 220, 200, fireRed, Color.WHITE);
        addBtn(layout, "AIM", 8, RelativeLayout.ALIGN_PARENT_RIGHT, RelativeLayout.ALIGN_PARENT_BOTTOM, 450, 200, espressoCard, accentGold);
        addBtn(layout, "JUMP", 5, RelativeLayout.ALIGN_PARENT_RIGHT, RelativeLayout.ALIGN_PARENT_BOTTOM, 50, 400, espressoCard, accentGold);
        addBtn(layout, "CROUCH", 6, RelativeLayout.ALIGN_PARENT_RIGHT, RelativeLayout.ALIGN_PARENT_BOTTOM, 50, 50, espressoCard, accentGold);
        
        // ATAS: Menu & Weapon
        addBtn(layout, "WEAPON", 9, RelativeLayout.ALIGN_PARENT_RIGHT, RelativeLayout.ALIGN_PARENT_TOP, 50, 50, espressoCard, accentGold);
        addBtn(layout, "VEHICLE", 10, RelativeLayout.ALIGN_PARENT_RIGHT, RelativeLayout.ALIGN_PARENT_TOP, 250, 50, espressoCard, accentGold);
    }

    private void addBtn(RelativeLayout layout, String txt, int code, int h, int v, int mx, int my, int bg, int tc) {
        Button b = new Button(this);
        b.setText(txt);
        b.setTextColor(tc);
        b.setBackgroundColor(bg);
        b.setTextSize(10);

        DisplayMetrics dm = getResources().getDisplayMetrics();
        RelativeLayout.LayoutParams p = new RelativeLayout.LayoutParams((int)(160*dm.density), (int)(110*dm.density));
        p.addRule(h); p.addRule(v);
        
        if (h == RelativeLayout.ALIGN_PARENT_LEFT) p.leftMargin = (int)(mx*dm.density);
        else p.rightMargin = (int)(mx*dm.density);
        
        if (v == RelativeLayout.ALIGN_PARENT_TOP) p.topMargin = (int)(my*dm.density);
        else p.bottomMargin = (int)(my*dm.density);

        b.setLayoutParams(p);
        b.setOnTouchListener((view, event) -> {
            if (event.getAction() == MotionEvent.ACTION_DOWN) nativeUpdateInput(code, true);
            else if (event.getAction() == MotionEvent.ACTION_UP) nativeUpdateInput(code, false);
            return true;
        });
        layout.addView(b);
    }

    private void checkStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivity(intent);
            }
        }
    }

    private void hideSystemUI() {
        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_FULLSCREEN);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        nativeInitGL();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int w, int h) {
        nativeResizeGL(w, h);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        long now = System.nanoTime();
        float dt = (now - lastFrameTime) / 1000000000.0f;
        lastFrameTime = now;

        nativeOnUpdate(dt);
        nativeRenderGL();
    }
    }
