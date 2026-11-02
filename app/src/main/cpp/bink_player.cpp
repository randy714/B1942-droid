#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "B1942_BINK"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class BinkPlayer {
public:
    bool isPlaying = false;
    std::string currentMovie;

    void play(const std::string& path) {
        currentMovie = path;
        isPlaying = true;
        LOGI("Bink Video Playback Started: %s", path.c_str());
    }

    void stop() {
        if (isPlaying) {
            isPlaying = false;
            LOGI("Bink Video Playback Stopped/Skipped.");
        }
    }
};

static BinkPlayer g_binkPlayer;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativePlayBink(JNIEnv* env, jobject thiz, jstring moviePath) {
    const char* path = env->GetStringUTFChars(moviePath, nullptr);
    g_binkPlayer.play(std::string(path));
    env->ReleaseStringUTFChars(moviePath, path);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeStopBink(JNIEnv* env, jobject thiz) {
    g_binkPlayer.stop();
}
