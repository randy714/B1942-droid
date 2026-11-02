package com.b1942.engine;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.*;
import android.view.MotionEvent;
import android.view.View;

public class TouchControlsView extends View {
    private Paint octPaint, firePaint, textPaint, strokePaint;
    private float scale = 1.0f;
    private boolean isEditor = false;

    // Posisi Tombol
    public float dpadX = 200, dpadY = 550;
    public float fireX = 1100, fireY = 550;
    public float aimX = 1100, aimY = 350;
    public float jumpX = 950, jumpY = 580;
    public float crouchX = 950, crouchY = 450;

    public TouchControlsView(Context context) {
        super(context);
        SharedPreferences prefs = context.getSharedPreferences("B1942_Prefs", Context.MODE_PRIVATE);
        loadPos(prefs);
        init();
    }

    private void init() {
        octPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        octPaint.setColor(Color.parseColor("#4A3525"));
        firePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        firePaint.setColor(Color.parseColor("#8B1A1A"));
        strokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        strokePaint.setColor(Color.parseColor("#D48C46"));
        strokePaint.setStyle(Paint.Style.STROKE);
        strokePaint.setStrokeWidth(3f);
        textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        textPaint.setColor(Color.WHITE);
        textPaint.setTextAlign(Paint.Align.CENTER);
        textPaint.setTextSize(24f);
    }

    public void setEditor(boolean mode) { this.isEditor = mode; }

    private void loadPos(SharedPreferences p) {
        dpadX = p.getFloat("px_dpad", 200); dpadY = p.getFloat("py_dpad", 550);
        fireX = p.getFloat("px_fire", 1100); fireY = p.getFloat("py_fire", 550);
        aimX = p.getFloat("px_aim", 1100); aimY = p.getFloat("py_aim", 350);
    }

    public void savePos(SharedPreferences p) {
        p.edit().putFloat("px_dpad", dpadX).putFloat("py_dpad", dpadY)
               .putFloat("px_fire", fireX).putFloat("py_fire", fireY)
               .putFloat("px_aim", aimX).putFloat("py_aim", aimY).apply();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        drawOct(canvas, dpadX, dpadY, 100, octPaint, "MOVE");
        canvas.drawCircle(fireX, fireY, 80, firePaint);
        canvas.drawCircle(fireX, fireY, 80, strokePaint);
        canvas.drawText("FIRE", fireX, fireY + 10, textPaint);
        drawOct(canvas, aimX, aimY, 65, octPaint, "AIM");
        drawOct(canvas, jumpX, jumpY, 55, octPaint, "JUMP");
        drawOct(canvas, crouchX, crouchY, 55, octPaint, "CROUCH");
    }

    private void drawOct(Canvas canvas, float cx, float cy, float r, Paint p, String txt) {
        Path path = new Path();
        for (int i = 0; i < 8; i++) {
            double a = Math.toRadians(45 * i - 22.5);
            float x = (float)(cx + r * Math.cos(a)), y = (float)(cy + r * Math.sin(a));
            if (i == 0) path.moveTo(x, y); else path.lineTo(x, y);
        }
        path.close();
        canvas.drawPath(path, p);
        canvas.drawPath(path, strokePaint);
        canvas.drawText(txt, cx, cy + 10, textPaint);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        if (!isEditor) return true;
        if (e.getAction() == MotionEvent.ACTION_MOVE) {
            float tx = e.getX(), ty = e.getY();
            if (Math.hypot(tx - fireX, ty - fireY) < 150) { fireX = tx; fireY = ty; }
            else if (Math.hypot(tx - dpadX, ty - dpadY) < 150) { dpadX = tx; dpadY = ty; }
            invalidate();
        }
        return true;
    }
                           }
