package com.b1942.engine;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class FileBrowserActivity extends Activity {

    private TextView tvCurrentPath;
    private ListView fileListView;
    private Button btnSelectFolder;
    private File currentDir;
    private List<String> dirList = new ArrayList<>();
    private ArrayAdapter<String> adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_browser);

        tvCurrentPath = findViewById(R.id.tvCurrentPath);
        fileListView = findViewById(R.id.fileListView);
        btnSelectFolder = findViewById(R.id.btnSelectCurrentFolder);

        String rootPath = Environment.getExternalStorageDirectory().getPath();
        SharedPreferences prefs = getSharedPreferences("B1942_Prefs", Context.MODE_PRIVATE);
        String savedPath = prefs.getString("custom_game_path", rootPath + "/B1942-droid/game");

        currentDir = new File(savedPath);
        if (!currentDir.exists()) {
            currentDir = new File(rootPath);
        }

        adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, dirList);
        fileListView.setAdapter(adapter);

        fileListView.setOnItemClickListener((parent, view, position, id) -> {
            String selected = dirList.get(position);
            if (selected.equals(".. [Parent Directory]")) {
                if (currentDir.getParentFile() != null) {
                    currentDir = currentDir.getParentFile();
                }
            } else {
                File nextDir = new File(currentDir, selected);
                if (nextDir.isDirectory()) {
                    currentDir = nextDir;
                }
            }
            updateDirList();
        });

        btnSelectFolder.setOnClickListener(v -> {
            SharedPreferences.Editor ed = prefs.edit();
            ed.putString("custom_game_path", currentDir.getAbsolutePath());
            ed.apply();
            Toast.makeText(this, "Game path set to: " + currentDir.getAbsolutePath(), Toast.LENGTH_SHORT).show();
            finish();
        });

        updateDirList();
    }

    private void updateDirList() {
        tvCurrentPath.setText(currentDir.getAbsolutePath());
        dirList.clear();

        if (currentDir.getParentFile() != null) {
            dirList.add(".. [Parent Directory]");
        }

        File[] files = currentDir.listFiles();
        if (files != null) {
            List<String> folders = new ArrayList<>();
            for (File f : files) {
                if (f.isDirectory() && !f.isHidden()) {
                    folders.add(f.getName());
                }
            }
            Collections.sort(folders);
            dirList.addAll(folders);
        }
        adapter.notifyDataSetChanged();
    }
          }
