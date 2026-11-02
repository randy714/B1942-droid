package com.b1942.engine;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.*;
import android.widget.FrameLayout;

public class GameActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
        FrameLayout root = new FrameLayout(this);
        root.setBackgroundColor(Color.parseColor("#121820")); // World Color

        // Kontrol Overlay
        TouchControlsView controls = new TouchControlsView(this);
        root.addView(controls);
        
        setContentView(root);
    }
}
