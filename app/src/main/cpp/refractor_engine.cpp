#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <android/log.h>

#define LOG_TAG "B1942_ENGINE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class RefractorEngine {
public:
    std::string basePath;
    bool initialized = false;

    void init(const std::string& path) {
        basePath = path;
        initialized = true;
        LOGI("Refractor 1 Engine Core Initialized at: %s", basePath.c_str());
        loadRfaArchive("Archives/standardMesh.rfa");
        loadRfaArchive("Archives/texture.rfa");
    }

    void loadRfaArchive(const std::string& relativePath) {
        std::string fullPath = basePath + "/" + relativePath;
        std::ifstream file(fullPath, std::ios::binary);
        if (file.is_open()) {
            LOGI("Successfully mounted archive: %s", relativePath.c_str());
            file.close();
        } else {
            LOGE("Warning: Archive not found: %s", relativePath.c_str());
        }
    }

    void handleInput(int actionId, bool pressed) {
        if (!initialized) return;
        LOGI("Input Action ID: %d | State: %s", actionId, pressed ? "DOWN" : "UP");
    }
};

static RefractorEngine g_engine;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInit(JNIEnv* env, jobject thiz, jstring path) {
    const char* nativePath = env->GetStringUTFChars(path, nullptr);
    g_engine.init(std::string(nativePath));
    env->ReleaseStringUTFChars(path, nativePath);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGameControl(JNIEnv* env, jobject thiz, jint actionId, jboolean pressed) {
    g_engine.handleInput(actionId, pressed);
}
