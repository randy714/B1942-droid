package com.b1942.engine;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;

public class MainActivity extends Activity {

    private TextView tvEngineStatus, tvStatusHeader;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tvEngineStatus = findViewById(R.id.tvEngineStatus);
        tvStatusHeader = findViewById(R.id.tvStatusHeader);

        Button btnLaunch = findViewById(R.id.btnLaunchGame);
        Button btnPlayMap = findViewById(R.id.btnPlayMap);
        Button btnLan = findViewById(R.id.btnLanMultiplayer);

        btnLaunch.setOnClickListener(v -> startActivity(new Intent(this, GameActivity.class)));
        btnPlayMap.setOnClickListener(v -> startActivity(new Intent(this, GameActivity.class)));
        btnLan.setOnClickListener(v -> Toast.makeText(this, "LAN Multiplayer requires local WiFi network", Toast.LENGTH_SHORT).show());

        findViewById(R.id.btnSelectFolder).setOnClickListener(v -> startActivity(new Intent(this, FileBrowserActivity.class)));
        findViewById(R.id.btnDisplayControls).setOnClickListener(v -> startActivity(new Intent(this, SettingsActivity.class)));
        findViewById(R.id.btnControlLayout).setOnClickListener(v -> startActivity(new Intent(this, ControlLayoutActivity.class)));

        findViewById(R.id.btnModManager).setOnClickListener(v -> Toast.makeText(this, "Mod Manager: Base game active", Toast.LENGTH_SHORT).show());
        findViewById(R.id.btnLaunchMod).setOnClickListener(v -> startActivity(new Intent(this, FileBrowserActivity.class)));
    }

    @Override
    protected void onResume() {
        super.onResume();
        SharedPreferences prefs = getSharedPreferences("B1942_Prefs", MODE_PRIVATE);
        String gamePath = prefs.getString("game_path", "");

        if (!gamePath.isEmpty() && new File(gamePath).exists()) {
            tvStatusHeader.setText("READY");
            tvEngineStatus.setText("Ready to play (Folder: " + new File(gamePath).getName() + ")");
        } else {
            tvStatusHeader.setText("DATA NEEDED");
            tvEngineStatus.setText("Select game folder in 'Play from a folder'");
        }
    }
          }
