package com.b1942.engine;

import android.os.Environment;
import java.io.File;

public class EngineExtractor {
    public static String getGameDir() {
        return Environment.getExternalStorageDirectory().getPath() + "/B1942-droid/game";
    }
}
