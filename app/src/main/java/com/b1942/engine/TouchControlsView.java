package com.b1942.engine;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class TouchControlsView extends View {

    public static final int NUM_BUTTONS = 10;
    public static final String[] BTN_NAMES = {
        "MOVE", "FIRE", "AIM", "CROUCH", "JUMP", "RELOAD", "WEAPON", "VEHICLE", "SCORE", "MENU"
    };

    private final float[] posX = new float[NUM_BUTTONS];
    private final float[] posY = new float[NUM_BUTTONS];
    private final float[] radius = new float[NUM_BUTTONS];
    private final boolean[] isPressed = new boolean[NUM_BUTTONS];

    private Paint fillPaint;
    private Paint strokePaint;
    private Paint textPaint;
    private Paint fireFillPaint;
    private Paint fireStrokePaint;

    private boolean editMode = false;
    private int selectedButtonIndex = -1;

    public TouchControlsView(Context context) {
        super(context);
        init();
    }

    public TouchControlsView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        fillPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        fillPaint.setColor(0x801E1612);
        fillPaint.setStyle(Paint.Style.FILL);

        strokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        strokePaint.setColor(0xFFD48C46);
        strokePaint.setStyle(Paint.Style.STROKE);
        strokePaint.setStrokeWidth(4f);

        textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        textPaint.setColor(0xFFE0A96D);
        textPaint.setTextAlign(Paint.Align.CENTER);
        textPaint.setTextSize(26f);
        textPaint.setFakeBoldText(true);

        fireFillPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        fireFillPaint.setColor(0x998B1A1A);
        fireFillPaint.setStyle(Paint.Style.FILL);

        fireStrokePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        fireStrokePaint.setColor(0xFFFF4444);
        fireStrokePaint.setStyle(Paint.Style.STROKE);
        fireStrokePaint.setStrokeWidth(5f);

        loadPositions();
    }

    public void setEditMode(boolean edit) {
        this.editMode = edit;
        invalidate();
    }

    public void loadPositions() {
        SharedPreferences prefs = getContext().getSharedPreferences("B1942_Prefs", Context.MODE_PRIVATE);

        // Standard Default Screen Layout Coordinates (Landscape Viewport Baseline)
        float[] defX = {180, 1100, 1100, 950, 1100, 950, 800, 950, 100, 1200};
        float[] defY = {520,  520,  360, 520, 200, 360, 520, 200,  80,   80};
        float[] defR = {110,   85,   65,  65,  65,  60,  60,  60,  50,   50};

        for (int i = 0; i < NUM_BUTTONS; i++) {
            posX[i] = prefs.getFloat("btn_" + i + "_x", defX[i]);
            posY[i] = prefs.getFloat("btn_" + i + "_y", defY[i]);
            radius[i] = prefs.getFloat("btn_" + i + "_r", defR[i]);
        }
        invalidate();
    }

    public void savePositions() {
        SharedPreferences.Editor ed = getContext().getSharedPreferences("B1942_Prefs", Context.MODE_PRIVATE).edit();
        for (int i = 0; i < NUM_BUTTONS; i++) {
            ed.putFloat("btn_" + i + "_x", posX[i]);
            ed.putFloat("btn_" + i + "_y", posY[i]);
            ed.putFloat("btn_" + i + "_r", radius[i]);
        }
        ed.apply();
    }

    public void resetToDefault() {
        SharedPreferences.Editor ed = getContext().getSharedPreferences("B1942_Prefs", Context.MODE_PRIVATE).edit();
        for (int i = 0; i < NUM_BUTTONS; i++) {
            ed.remove("btn_" + i + "_x");
            ed.remove("btn_" + i + "_y");
            ed.remove("btn_" + i + "_r");
        }
        ed.apply();
        loadPositions();
    }

    private Path createOctagonPath(float cx, float cy, float r) {
        Path path = new Path();
        float side = r * 0.41421356f;
        path.moveTo(cx - side, cy - r);
        path.lineTo(cx + side, cy - r);
        path.lineTo(cx + r, cy - side);
        path.lineTo(cx + r, cy + side);
        path.lineTo(cx + side, cy + r);
        path.lineTo(cx - side, cy + r);
        path.lineTo(cx - r, cy + side);
        path.lineTo(cx - r, cy - side);
        path.close();
        return path;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        for (int i = 0; i < NUM_BUTTONS; i++) {
            float x = posX[i];
            float y = posY[i];
            float r = radius[i];

            if (i == 1) { // FIRE Button: Tactical Red Circle
                canvas.drawCircle(x, y, r, fireFillPaint);
                canvas.drawCircle(x, y, r, fireStrokePaint);
            } else { // Standard Action Buttons: Octagonal Shape
                Path oct = createOctagonPath(x, y, r);
                canvas.drawPath(oct, fillPaint);

                if (editMode && i == selectedButtonIndex) {
                    Paint highlight = new Paint(strokePaint);
                    highlight.setColor(0xFFFFFFFF);
                    highlight.setStrokeWidth(6f);
                    canvas.drawPath(oct, highlight);
                } else {
                    canvas.drawPath(oct, strokePaint);
                }
            }

            canvas.drawText(BTN_NAMES[i], x, y + (r * 0.15f), textPaint);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked();
        int pointerIndex = event.getActionIndex();
        float tx = event.getX(pointerIndex);
        float ty = event.getY(pointerIndex);

        if (editMode) {
            switch (action) {
                case MotionEvent.ACTION_DOWN:
                    selectedButtonIndex = findTouchedButton(tx, ty);
                    break;
                case MotionEvent.ACTION_MOVE:
                    if (selectedButtonIndex != -1) {
                        posX[selectedButtonIndex] = tx;
                        posY[selectedButtonIndex] = ty;
                        invalidate();
                    }
                    break;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    selectedButtonIndex = -1;
                    invalidate();
                    break;
            }
            return true;
        } else {
            // Gameplay touch handling for all 10 buttons
            switch (action) {
                case MotionEvent.ACTION_DOWN:
                case MotionEvent.ACTION_POINTER_DOWN:
                    int btnDown = findTouchedButton(tx, ty);
                    if (btnDown != -1) {
                        isPressed[btnDown] = true;
                        NativeEngine.nativeGameControl(btnDown, true);
                        invalidate();
                    }
                    break;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_POINTER_UP:
                case MotionEvent.ACTION_CANCEL:
                    int btnUp = findTouchedButton(tx, ty);
                    if (btnUp != -1) {
                        isPressed[btnUp] = false;
                        NativeEngine.nativeGameControl(btnUp, false);
                        invalidate();
                    }
                    break;
            }
            return true;
        }
    }

    private int findTouchedButton(float x, float y) {
        for (int i = 0; i < NUM_BUTTONS; i++) {
            float dx = x - posX[i];
            float dy = y - posY[i];
            float distSq = dx * dx + dy * dy;
            if (distSq <= radius[i] * radius[i] * 1.5f) {
                return i;
            }
        }
        return -1;
    }

    public void resizeSelectedButton(float delta) {
        if (selectedButtonIndex != -1) {
            radius[selectedButtonIndex] = Math.max(30f, Math.min(200f, radius[selectedButtonIndex] + delta));
            invalidate();
        }
    }
          }
