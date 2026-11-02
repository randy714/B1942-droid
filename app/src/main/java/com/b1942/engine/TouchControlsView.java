package com.b1942.engine;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.view.MotionEvent;
import android.view.View;

public class TouchControlsView extends View {

    private Paint octPaint, firePaint, textPaint, strokePaint, aimPaint;
    private float buttonScale = 1.0f;

    public TouchControlsView(Context context) {
        super(context);
        initPaints();
    }

    private void initPaints() {
        octPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        octPaint.setColor(Color.parseColor("#4A3525"));
        octPaint.setStyle(Paint.Style.FILL);

        strokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        strokePaint.setColor(Color.parseColor("#D48C46"));
        strokePaint.setStyle(Paint.Style.STROKE);
        strokePaint.setStrokeWidth(4f);

        firePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        firePaint.setColor(Color.parseColor("#8B1A1A"));
        firePaint.setStyle(Paint.Style.FILL);

        aimPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        aimPaint.setColor(Color.parseColor("#1E1612"));
        aimPaint.setStyle(Paint.Style.FILL);

        textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        textPaint.setColor(Color.parseColor("#F5EBE6"));
        textPaint.setTextSize(26f);
        textPaint.setTextAlign(Paint.Align.CENTER);
        textPaint.setFakeBoldText(true);
    }

    public void setCustomScale(float scale) {
        this.buttonScale = scale;
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        int h = getHeight();
        int w = getWidth();
        if (h == 0 || w == 0) return;

        // 1. Move D-Pad (Octagon - Left)
        drawOctagon(canvas, 180 * buttonScale, h - (180 * buttonScale), 100 * buttonScale, octPaint, strokePaint);
        canvas.drawText("MOVE", 180 * buttonScale, h - (170 * buttonScale), textPaint);

        // 2. FIRE BUTTON (Red Circle - Right)
        float fireX = w - (180 * buttonScale);
        float fireY = h - (180 * buttonScale);
        canvas.drawCircle(fireX, fireY, 80 * buttonScale, firePaint);
        canvas.drawCircle(fireX, fireY, 80 * buttonScale, strokePaint);
        canvas.drawText("FIRE", fireX, fireY + 10, textPaint);

        // 3. AIM / SCOPE Button (Octagon - Above Fire)
        drawOctagon(canvas, w - (180 * buttonScale), h - (360 * buttonScale), 60 * buttonScale, aimPaint, strokePaint);
        canvas.drawText("AIM", w - (180 * buttonScale), h - (350 * buttonScale), textPaint);

        // 4. JUMP Button
        drawOctagon(canvas, w - (340 * buttonScale), h - (160 * buttonScale), 55 * buttonScale, octPaint, strokePaint);
        canvas.drawText("JUMP", w - (340 * buttonScale), h - (150 * buttonScale), textPaint);

        // 5. CROUCH Button
        drawOctagon(canvas, w - (340 * buttonScale), h - (300 * buttonScale), 55 * buttonScale, octPaint, strokePaint);
        canvas.drawText("CROUCH", w - (340 * buttonScale), h - (290 * buttonScale), textPaint);

        // 6. RELOAD Button
        drawOctagon(canvas, w - (180 * buttonScale), h - (510 * buttonScale), 50 * buttonScale, octPaint, strokePaint);
        canvas.drawText("RELOAD", w - (180 * buttonScale), h - (500 * buttonScale), textPaint);

        // 7. ENTER VEHICLE / USE Button
        drawOctagon(canvas, 380 * buttonScale, h - (160 * buttonScale), 60 * buttonScale, octPaint, strokePaint);
        canvas.drawText("ENTER", 380 * buttonScale, h - (150 * buttonScale), textPaint);

        // 8. SWITCH WEAPON Button
        drawOctagon(canvas, w - (340 * buttonScale), h - (440 * buttonScale), 50 * buttonScale, octPaint, strokePaint);
        canvas.drawText("WEAPON", w - (340 * buttonScale), h - (430 * buttonScale), textPaint);

        // 9. TOP BAR: SCOREBOARD & PAUSE
        drawOctagon(canvas, 120, 60, 45, octPaint, strokePaint);
        canvas.drawText("SCORE", 120, 70, textPaint);

        drawOctagon(canvas, w - 120, 60, 45, octPaint, strokePaint);
        canvas.drawText("MENU", w - 120, 70, textPaint);
    }

    private void drawOctagon(Canvas canvas, float cx, float cy, float radius, Paint fill, Paint stroke) {
        Path path = new Path();
        for (int i = 0; i < 8; i++) {
            double angle = Math.toRadians(45 * i - 22.5);
            float x = (float) (cx + radius * Math.cos(angle));
            float y = (float) (cy + radius * Math.sin(angle));
            if (i == 0) path.moveTo(x, y);
            else path.lineTo(x, y);
        }
        path.close();
        canvas.drawPath(path, fill);
        canvas.drawPath(path, stroke);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return true;
    }
            }
