package com.b1942.engine;

import android.content.Context;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class EngineExtractor {

    public static File getInternalEngineDir(Context context) {
        File engineDir = new File(context.getFilesDir(), "engine_runtime");
        if (!engineDir.exists()) {
            engineDir.mkdirs();
        }
        return engineDir;
    }

    public static boolean prepareEmbeddedEngine(Context context) {
        File engineDir = getInternalEngineDir(context);
        File configFile = new File(engineDir, "engine_config.ini");
        
        if (!configFile.exists()) {
            try {
                FileOutputStream fos = new FileOutputStream(configFile);
                String cfg = "[Engine]\nVersion=2.5-B1942-DROID\nRender=OpenGL_ES3\n";
                fos.write(cfg.getBytes());
                fos.close();
                return true;
            } catch (IOException e) {
                return false;
            }
        }
        return true;
    }
}
