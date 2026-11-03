#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "B1942_BINK"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class BinkMoviePlayer {
public:
    bool isPlaying = false;
    std::string currentPath;

    void playMovie(const std::string& path) {
        currentPath = path;
        isPlaying = true;
        LOGI("Playing Bink Movie: %s", currentPath.c_str());
    }

    void stopMovie() {
        if (isPlaying) {
            isPlaying = false;
            LOGI("Bink Movie Playback Stopped / Skipped.");
        }
    }
};

static BinkMoviePlayer g_binkPlayer;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativePlayBink(JNIEnv* env, jobject thiz, jstring moviePath) {
    if (!moviePath) return;
    const char* path = env->GetStringUTFChars(moviePath, nullptr);
    if (path) {
        g_binkPlayer.playMovie(std::string(path));
        env->ReleaseStringUTFChars(moviePath, path);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeStopBink(JNIEnv* env, jobject thiz) {
    g_binkPlayer.stopMovie();
}
