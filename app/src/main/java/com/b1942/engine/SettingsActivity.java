package com.b1942.engine;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class SettingsActivity extends Activity {

    private Spinner spLanguage, spFpsLimit, spResolution, spRenderer, spRamAlloc;
    private SeekBar sbOpacity, sbScale;
    private TextView tvOpacityLabel, tvScaleLabel;
    private SharedPreferences prefs;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        prefs = getSharedPreferences("B1942_Prefs", MODE_PRIVATE);

        spLanguage = findViewById(R.id.spLanguage);
        spFpsLimit = findViewById(R.id.spFpsLimit);
        spResolution = findViewById(R.id.spResolution);
        spRenderer = findViewById(R.id.spRenderer);
        spRamAlloc = findViewById(R.id.spRamAlloc);
        sbOpacity = findViewById(R.id.sbOpacity);
        sbScale = findViewById(R.id.sbScale);
        tvOpacityLabel = findViewById(R.id.tvOpacityLabel);
        tvScaleLabel = findViewById(R.id.tvScaleLabel);
        Button btnSave = findViewById(R.id.btnSaveSettings);

        setupSpinners();
        loadSavedSettings();

        sbOpacity.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                tvOpacityLabel.setText("Button Opacity: " + progress + "%");
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        sbScale.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                tvScaleLabel.setText("Button Scale: " + progress + "%");
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        btnSave.setOnClickListener(v -> saveSettings());
    }

    private void setupSpinners() {
        String[] languages = {"Bahasa Indonesia", "English", "Español", "Русский", "Deutsch", "Français"};
        String[] fpsOptions = {"30 FPS", "60 FPS", "90 FPS", "120 FPS", "Unlimited"};
        String[] resolutions = {"Native (100%)", "75% Render Scale", "50% Performance Scale", "125% HD Scale"};
        String[] renderers = {"OpenGL ES 3.0 (Recommended)", "Vulkan Engine (Experimental)", "Software Engine"};
        String[] ramOptions = {"2 GB RAM", "3 GB RAM", "4 GB RAM", "6 GB RAM"};

        spLanguage.setAdapter(new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, languages));
        spFpsLimit.setAdapter(new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, fpsOptions));
        spResolution.setAdapter(new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, resolutions));
        spRenderer.setAdapter(new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, renderers));
        spRamAlloc.setAdapter(new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, ramOptions));
    }

    private void loadSavedSettings() {
        spLanguage.setSelection(prefs.getInt("language_idx", 0));
        spFpsLimit.setSelection(prefs.getInt("fps_idx", 1));
        spResolution.setSelection(prefs.getInt("res_idx", 0));
        spRenderer.setSelection(prefs.getInt("renderer_idx", 0));
        spRamAlloc.setSelection(prefs.getInt("ram_idx", 2));

        int opacity = prefs.getInt("btn_opacity", 80);
        int scale = prefs.getInt("btn_scale", 100);
        sbOpacity.setProgress(opacity);
        sbScale.setProgress(scale);
        tvOpacityLabel.setText("Button Opacity: " + opacity + "%");
        tvScaleLabel.setText("Button Scale: " + scale + "%");
    }

    private void saveSettings() {
        SharedPreferences.Editor editor = prefs.edit();
        editor.putInt("language_idx", spLanguage.getSelectedItemPosition());
        editor.putInt("fps_idx", spFpsLimit.getSelectedItemPosition());
        editor.putInt("res_idx", spResolution.getSelectedItemPosition());
        editor.putInt("renderer_idx", spRenderer.getSelectedItemPosition());
        editor.putInt("ram_idx", spRamAlloc.getSelectedItemPosition());
        editor.putInt("btn_opacity", sbOpacity.getProgress());
        editor.putInt("btn_scale", sbScale.getProgress());
        editor.apply();

        Toast.makeText(this, "Settings Saved Successfully!", Toast.LENGTH_SHORT).show();
        finish();
    }
}
