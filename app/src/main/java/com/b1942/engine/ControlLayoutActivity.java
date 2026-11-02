package com.b1942.engine;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.Toast;

public class ControlLayoutActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control_layout);

        FrameLayout container = findViewById(R.id.layoutEditorContainer);
        TouchControlsView controls = new TouchControlsView(this);
        container.addView(controls);

        Button btnSave = findViewById(R.id.btnSaveLayout);
        Button btnReset = findViewById(R.id.btnResetLayout);

        btnSave.setOnClickListener(v -> {
            Toast.makeText(this, "Control layout saved!", Toast.LENGTH_SHORT).show();
            finish();
        });

        btnReset.setOnClickListener(v -> Toast.makeText(this, "Layout reset to default", Toast.LENGTH_SHORT).show());
    }
}
