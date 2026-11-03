#include <jni.h>
#include <string>
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "B1942_BINK", __VA_ARGS__)

class BinkMoviePlayer {
public:
    bool isPlaying = false;
    void play(const std::string& p) { isPlaying = true; LOGI("Playing: %s", p.c_str()); }
    void stop() { if (isPlaying) { isPlaying = false; LOGI("Playback Skipped."); } }
};

static BinkMoviePlayer g_bink;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativePlayBink(JNIEnv* env, jobject thiz, jstring path) {
    const char* p = env->GetStringUTFChars(path, nullptr);
    g_bink.play(std::string(p));
    env->ReleaseStringUTFChars(path, p);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeStopBink(JNIEnv* env, jobject thiz) { g_bink.stop(); }
