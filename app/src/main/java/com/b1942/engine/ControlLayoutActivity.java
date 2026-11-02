package com.b1942.engine;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

public class ControlLayoutActivity extends Activity {

    private TouchControlsView editorView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_control_layout);

        editorView = findViewById(R.id.touchControlsEditor);
        editorView.setEditMode(true);

        Button btnSave = findViewById(R.id.btnSaveLayout);
        Button btnReset = findViewById(R.id.btnResetLayout);
        Button btnPlus = findViewById(R.id.btnSizePlus);
        Button btnMinus = findViewById(R.id.btnSizeMinus);

        btnSave.setOnClickListener(v -> {
            editorView.savePositions();
            Toast.makeText(this, "Layout Saved!", Toast.LENGTH_SHORT).show();
            finish();
        });

        btnReset.setOnClickListener(v -> {
            editorView.resetToDefault();
            Toast.makeText(this, "Layout Reset to Default", Toast.LENGTH_SHORT).show();
        });

        btnPlus.setOnClickListener(v -> editorView.resizeSelectedButton(10f));
        btnMinus.setOnClickListener(v -> editorView.resizeSelectedButton(-10f));
    }
}
