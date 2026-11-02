package com.b1942.engine;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tvStatusHeader = findViewById(R.id.tvStatusHeader);
        TextView tvEngineStatus = findViewById(R.id.tvEngineStatus);

        findViewById(R.id.btnLaunchGame).setOnClickListener(v -> 
            startActivity(new Intent(this, GameActivity.class)));

        findViewById(R.id.btnPlayMap).setOnClickListener(v -> 
            startActivity(new Intent(this, GameActivity.class)));

        findViewById(R.id.btnLanMultiplayer).setOnClickListener(v -> 
            Toast.makeText(this, "Multiplayer coming soon", Toast.LENGTH_SHORT).show());

        findViewById(R.id.btnSelectFolder).setOnClickListener(v -> 
            startActivity(new Intent(this, FileBrowserActivity.class)));

        findViewById(R.id.btnDisplayControls).setOnClickListener(v -> 
            startActivity(new Intent(this, SettingsActivity.class)));

        findViewById(R.id.btnControlLayout).setOnClickListener(v -> 
            startActivity(new Intent(this, ControlLayoutActivity.class)));

        findViewById(R.id.btnModManager).setOnClickListener(v -> 
            Toast.makeText(this, "Mod Manager active", Toast.LENGTH_SHORT).show());

        findViewById(R.id.btnLaunchMod).setOnClickListener(v -> 
            Toast.makeText(this, "Select mod folder", Toast.LENGTH_SHORT).show());
    }
  }
