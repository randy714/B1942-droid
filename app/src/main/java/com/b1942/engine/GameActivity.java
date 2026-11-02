package com.b1942.engine;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.Toast;
import java.io.File;

public class GameActivity extends Activity {

    private TouchControlsView touchControlsView;
    private FrameLayout gameSurfaceLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_game);
        gameSurfaceLayout = findViewById(R.id.gameSurfaceLayout);

        SharedPreferences prefs = getSharedPreferences("B1942_Prefs", MODE_PRIVATE);
        String gameDataPath = prefs.getString("game_path", "");

        if (gameDataPath.isEmpty()) {
            Toast.makeText(this, "Please select game folder in MODS/DATA menu!", Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        Toast.makeText(this, "Starting B1942-droid Engine...\nData: " + gameDataPath, Toast.LENGTH_SHORT).show();
        setupTouchOverlay();
    }

    private void setupTouchOverlay() {
        touchControlsView = new TouchControlsView(this);
        SharedPreferences prefs = getSharedPreferences("B1942_Prefs", MODE_PRIVATE);

        int opacity = prefs.getInt("btn_opacity", 80);
        int scale = prefs.getInt("btn_scale", 100);

        touchControlsView.setAlpha(opacity / 100.0f);
        touchControlsView.setScaleX(scale / 100.0f);
        touchControlsView.setScaleY(scale / 100.0f);

        gameSurfaceLayout.addView(touchControlsView);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            );
        }
    }
}
