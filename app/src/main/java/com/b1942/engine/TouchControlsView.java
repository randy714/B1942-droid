package com.b1942.engine;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.view.MotionEvent;
import android.view.View;

public class TouchControlsView extends View {

    private Paint octPaint, firePaint, textPaint;

    public TouchControlsView(Context context) {
        super(context);

        octPaint = new Paint();
        octPaint.setColor(Color.parseColor("#4A3525"));
        octPaint.setStyle(Paint.Style.FILL);

        firePaint = new Paint();
        firePaint.setColor(Color.parseColor("#8B1A1A")); // Red Fire Button
        firePaint.setStyle(Paint.Style.FILL);

        textPaint = new Paint();
        textPaint.setColor(Color.WHITE);
        textPaint.setTextSize(36f);
        textPaint.setTextAlign(Paint.Align.CENTER);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        int h = getHeight();
        int w = getWidth();

        // Octagonal D-PAD (Left Side)
        drawOctagon(canvas, 180, h - 180, 100, octPaint);
        canvas.drawText("MOVE", 180, h - 170, textPaint);

        // FIRE BUTTON (Right Side - Red)
        canvas.drawCircle(w - 180, h - 180, 80, firePaint);
        canvas.drawText("FIRE", w - 180, h - 170, textPaint);
    }

    private void drawOctagon(Canvas canvas, float cx, float cy, float radius, Paint paint) {
        Path path = new Path();
        for (int i = 0; i < 8; i++) {
            double angle = Math.toRadians(45 * i - 22.5);
            float x = (float) (cx + radius * Math.cos(angle));
            float y = (float) (cy + radius * Math.sin(angle));
            if (i == 0) path.moveTo(x, y);
            else path.lineTo(x, y);
        }
        path.close();
        canvas.drawPath(path, paint);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return true;
    }
}
