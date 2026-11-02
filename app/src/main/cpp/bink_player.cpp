#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "B1942_BINK"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativePlayBink(JNIEnv* env, jobject thiz, jstring moviePath) {
    if (!moviePath) return;
    const char* path = env->GetStringUTFChars(moviePath, nullptr);
    if (path) {
        LOGI("Playing Bink movie: %s", path);
        env->ReleaseStringUTFChars(moviePath, path);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeStopBink(JNIEnv* env, jobject thiz) {
    LOGI("Bink movie playback stopped.");
}
