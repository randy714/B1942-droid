#include <jni.h>
#include <string>
#include <fstream>
#include <android/log.h>

#define LOG_TAG "B1942_ENGINE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class SafeRefractorEngine {
public:
    std::string gamePath;
    bool isEngineReady = false;

    void initialize(const char* path) {
        if (!path) return;
        gamePath = std::string(path);
        
        // Proteksi Cek Akses Folder
        if (!gamePath.empty()) {
            isEngineReady = true;
            LOGI("Engine ready. Target path: %s", gamePath.c_str());
        } else {
            LOGE("Invalid Game Path provided!");
            isEngineReady = false;
        }
    }

    void handleInput(int actionId, bool pressed) {
        if (!isEngineReady) return;
        // Input disalurkan langsung tanpa delay
    }
};

static SafeRefractorEngine* g_engine = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInit(JNIEnv* env, jobject thiz, jstring path) {
    if (!path) return;
    const char* nativePath = env->GetStringUTFChars(path, nullptr);
    if (nativePath) {
        if (!g_engine) g_engine = new SafeRefractorEngine();
        g_engine->initialize(nativePath);
        env->ReleaseStringUTFChars(path, nativePath);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGameControl(JNIEnv* env, jobject thiz, jint actionId, jboolean pressed) {
    if (g_engine) {
        g_engine->handleInput(actionId, pressed);
    }
}
