package com.b1942.engine;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends Activity {

    private TextView tvEngineStatus;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tvEngineStatus = findViewById(R.id.tvEngineStatus);

        requestStoragePermissions();
        EngineExtractor.prepareEngine(this);

        Button btnSelectFolder = findViewById(R.id.btnSelectFolder);
        Button btnLaunchGame = findViewById(R.id.btnLaunchGame);
        Button btnControlLayout = findViewById(R.id.btnControlLayout);
        Button btnSettings = findViewById(R.id.btnDisplayControls);

        btnSelectFolder.setOnClickListener(v -> 
            startActivity(new Intent(this, FileBrowserActivity.class)));

        btnControlLayout.setOnClickListener(v -> 
            startActivity(new Intent(this, ControlLayoutActivity.class)));

        btnSettings.setOnClickListener(v -> 
            startActivity(new Intent(this, SettingsActivity.class)));

        btnLaunchGame.setOnClickListener(v -> {
            String gamePath = getGamePath();
            File gameFolder = new File(gamePath);

            if (!gameFolder.exists()) {
                Toast.makeText(this, "Game folder not found: " + gamePath, Toast.LENGTH_LONG).show();
                return;
            }

            Intent intent = new Intent(this, GameActivity.class);
            intent.putExtra("GAME_PATH", gamePath);
            startActivity(intent);
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateEngineStatus();
    }

    private String getGamePath() {
        SharedPreferences prefs = getSharedPreferences("B1942_Prefs", Context.MODE_PRIVATE);
        return prefs.getString("custom_game_path", EngineExtractor.getGameDir());
    }

    private void updateEngineStatus() {
        String path = getGamePath();
        File folder = new File(path);
        File modsFolder = new File(folder, "Mods/bf1942/Archives");

        if (modsFolder.exists() && modsFolder.isDirectory()) {
            tvEngineStatus.setText("Status: Game Archives Found (" + path + ")");
            tvEngineStatus.setTextColor(0xFFE0A96D);
        } else {
            tvEngineStatus.setText("Status: Archives Missing at " + path);
            tvEngineStatus.setTextColor(0xFFA08C80);
        }
    }

    private void requestStoragePermissions() {
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
                          }
