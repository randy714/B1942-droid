package com.b1942.engine;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.Toast;
import android.graphics.Color;

public class ControlLayoutActivity extends Activity {
    private TouchControlsView touchView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        FrameLayout root = new FrameLayout(this);
        root.setBackgroundColor(Color.BLACK);

        touchView = new TouchControlsView(this);
        touchView.setEditor(true);
        root.addView(touchView);

        Button btnSave = new Button(this);
        btnSave.setText("SAVE LAYOUT");
        btnSave.setOnClickListener(v -> {
            SharedPreferences prefs = getSharedPreferences("B1942_Prefs", MODE_PRIVATE);
            touchView.savePos(prefs);
            Toast.makeText(this, "Layout Saved!", Toast.LENGTH_SHORT).show();
            finish();
        });
        
        root.addView(btnSave, new FrameLayout.LayoutParams(400, 150, Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL));
        setContentView(root);
    }
}
