#include <jni.h>
#include <android/log.h>

#define LOG_TAG "B1942_Bink"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativePlayBink(JNIEnv* env, jobject thiz, jstring movie_path) {
    const char* path = env->GetStringUTFChars(movie_path, nullptr);
    LOGI("Bink Player: Membuka file video (.bik) -> %s", path);
    env->ReleaseStringUTFChars(movie_path, path);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeStopBink(JNIEnv* env, jobject thiz) {
    LOGI("Bink Player: Video (.bik) dihentikan.");
}
