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
        System.loadLibrary("b1942_engine");
    }

    // Native JNI Declarations
    public native void nativeInitEngine(String gamePath);
    public native void nativeUpdateInput(int keyCode, boolean isPressed);
    public native void nativeOnUpdate(float deltaTime);
    public native void nativeOnLook(float dx, float dy, float dt);
    public native void nativePlayBink(String moviePath);
    public native void nativeStopBink();
    public native void nativeInitGL();
    public native void nativeResizeGL(int w, int h);
    public native void nativeRenderGL();

    private GLSurfaceView glView;
    private long lastFrameTime = System.nanoTime();
    private float lastTouchX, lastTouchY;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            getWindow().getAttributes().layoutInDisplayCutoutMode =
                    WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
        }

        hideSystemUI();
        checkPermissions();

        FrameLayout mainLayout = new FrameLayout(this);

        // Setup OpenGL ES View
        glView = new GLSurfaceView(this);
        glView.setEGLContextClientVersion(2);
        glView.setRenderer(this);
        glView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        mainLayout.addView(glView);

        // Setup Touch Controls Overlays
        RelativeLayout controlsLayout = createTacticalTouchControls();
        mainLayout.addView(controlsLayout);

        setContentView(mainLayout);

        // Inisialisasi Refractor Engine Native dengan Path Game
        String gamePath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/B1942-droid/game";
        nativeInitEngine(gamePath);
        nativePlayBink(gamePath + "/Movies/intro.bik");
    }

    private void hideSystemUI() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }

    private void checkPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                try {
                    Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                    intent.setData(Uri.parse("package:" + getPackageName()));
                    startActivity(intent);
                } catch (Exception e) {
                    Intent intent = new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
                    startActivity(intent);
                }
            }
        }
    }

    private RelativeLayout createTacticalTouchControls() {
        RelativeLayout layout = new RelativeLayout(this);

        // Styling Warna Espresso
        int cardBg = Color.parseColor("#1E1612");
        int goldText = Color.parseColor("#E0A96D");
        int fireBg = Color.parseColor("#8B1A1A");

        // 10 Touch Buttons: MOVE_FWD, MOVE_BACK, MOVE_LEFT, MOVE_RIGHT, JUMP, CROUCH, FIRE, AIM, WEAPON, VEHICLE
        addButton(layout, "FWD", 1, 150, 1000, cardBg, goldText);
        addButton(layout, "BACK", 2, 150, 1220, cardBg, goldText);
        addButton(layout, "LEFT", 3, 40, 1110, cardBg, goldText);
        addButton(layout, "RIGHT", 4, 260, 1110, cardBg, goldText);

        addButton(layout, "JUMP", 5, 2000, 1000, cardBg, goldText);
        addButton(layout, "CROUCH", 6, 2000, 1200, cardBg, goldText);
        addButton(layout, "FIRE", 7, 1800, 1100, fireBg, Color.WHITE);
        addButton(layout, "AIM", 8, 1600, 1100, cardBg, goldText);
        addButton(layout, "WEAPON", 9, 2000, 300, cardBg, goldText);
        addButton(layout, "VEHICLE", 10, 2000, 100, cardBg, goldText);

        // Touch Control Kamera Look Panning
        layout.setOnTouchListener((v, event) -> {
            float x = event.getX();
            float y = event.getY();
            if (x > getWindowManager().getDefaultDisplay().getWidth() / 2) {
                if (event.getAction() == MotionEvent.ACTION_MOVE) {
                    float dx = x - lastTouchX;
                    float dy = y - lastTouchY;
                    nativeOnLook(dx, dy, 0.016f);
                }
                lastTouchX = x;
                lastTouchY = y;
            }
            return true;
        });

        return layout;
    }

    private void addButton(RelativeLayout layout, String text, int keyCode, int x, int y, int bgColor, int textColor) {
        Button btn = new Button(this);
        btn.setText(text);
        btn.setTextColor(textColor);
        btn.setBackgroundColor(bgColor);
        btn.setTextSize(12);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(180, 100);
        params.leftMargin = x;
        params.topMargin = y;
        btn.setLayoutParams(params);

        btn.setOnTouchListener((v, event) -> {
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                nativeUpdateInput(keyCode, true);
                nativeStopBink(); // Lewati video jika tombol disentuh
            } else if (event.getAction() == MotionEvent.ACTION_UP) {
                nativeUpdateInput(keyCode, false);
            }
            return true;
        });

        layout.addView(btn);
    }

    // GLSurfaceView Renderer Callbacks
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        nativeInitGL();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        nativeResizeGL(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        long currentTime = System.nanoTime();
        float deltaTime = (currentTime - lastFrameTime) / 1000000000.0f;
        lastFrameTime = currentTime;

        nativeOnUpdate(deltaTime);
        nativeRenderGL();
    }
}
