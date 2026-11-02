package com.b1942.engine;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
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
    private File currentDir;
    private List<String> fileNames = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_browser);

        tvCurrentPath = findViewById(R.id.tvCurrentPath);
        fileListView = findViewById(R.id.fileListView);
        Button btnSelect = findViewById(R.id.btnSelectCurrentFolder);

        currentDir = Environment.getExternalStorageDirectory();
        loadDirectory(currentDir);

        fileListView.setOnItemClickListener((parent, view, position, id) -> {
            String selected = fileNames.get(position);
            if (selected.equals(".. (Back)")) {
                if (currentDir.getParentFile() != null) {
                    currentDir = currentDir.getParentFile();
                    loadDirectory(currentDir);
                }
            } else {
                File clicked = new File(currentDir, selected);
                if (clicked.isDirectory()) {
                    currentDir = clicked;
                    loadDirectory(currentDir);
                }
            }
        });

        btnSelect.setOnClickListener(v -> {
            SharedPreferences prefs = getSharedPreferences("B1942_Prefs", MODE_PRIVATE);
            prefs.edit().putString("game_path", currentDir.getAbsolutePath()).apply();
            Toast.makeText(this, "Selected: " + currentDir.getName(), Toast.LENGTH_SHORT).show();
            finish();
        });
    }

    private void loadDirectory(File dir) {
        currentDir = dir;
        tvCurrentPath.setText(dir.getAbsolutePath());
        fileNames.clear();

        if (dir.getParentFile() != null) {
            fileNames.add(".. (Back)");
        }

        File[] files = dir.listFiles();
        if (files != null) {
            List<String> dirs = new ArrayList<>();
            for (File f : files) {
                if (f.isDirectory()) {
                    dirs.add(f.getName());
                }
            }
            Collections.sort(dirs);
            fileNames.addAll(dirs);
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, fileNames);
        fileListView.setAdapter(adapter);
    }
}
