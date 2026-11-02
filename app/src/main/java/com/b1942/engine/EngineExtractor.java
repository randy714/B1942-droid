package com.b1942.engine;

import android.content.Context;
import android.os.Environment;
import java.io.File;

public class EngineExtractor {

    public static String getGameDir() {
        return Environment.getExternalStorageDirectory().getPath() + "/B1942-droid/game";
    }

    public static void prepareEngine(Context context) {
        File folder = new File(getGameDir());
        if (!folder.exists()) {
            folder.mkdirs();
            new File(folder, "Mods/bf1942/Archives").mkdirs();
            new File(folder, "Settings").mkdirs();
        }

        try {
            System.loadLibrary("b1942_engine");
        } catch (UnsatisfiedLinkError e) {
            e.printStackTrace();
        }
    }
}
