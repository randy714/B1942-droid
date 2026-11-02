#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>
#include "gl_renderer.h"
#include "rfa_loader.h"

#define LOG_TAG "B1942_NativeEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static GLRenderer* g_Renderer = nullptr;
static std::vector<RFALoader*> g_Archives;

extern "C" JNIEXPORT jboolean JNICALL
Java_com_b1942_engine_NativeEngine_nativeInit(JNIEnv* env, jclass clazz, jstring gamePath, jint width, jint height) {
    const char* pathStr = env->GetStringUTFChars(gamePath, nullptr);
    std::string basePath(pathStr);
    LOGI("Initializing Engine Core with target path: %s", pathStr);

    // Buka berkas arsip utama Refractor (.rfa)
    std::vector<std::string> targetArchives = {
        basePath + "/Mods/bf1942/Archives/texture.rfa",
        basePath + "/Mods/bf1942/Archives/objects.rfa",
        basePath + "/Mods/bf1942/Archives/standardmesh.rfa",
        basePath + "/Mods/bf1942/Archives/menu.rfa"
    };

    for (const auto& archivePath : targetArchives) {
        RFALoader* loader = new RFALoader();
        if (loader->openArchive(archivePath)) {
            g_Archives.push_back(loader);
        } else {
            delete loader;
        }
    }

    if (!g_Renderer) g_Renderer = new GLRenderer();
    g_Renderer->init(width, height);

    env->ReleaseStringUTFChars(gamePath, pathStr);
    return JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_NativeEngine_nativeResize(JNIEnv* env, jclass clazz, jint width, jint height) {
    if (g_Renderer) g_Renderer->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_NativeEngine_nativeRender(JNIEnv* env, jclass clazz) {
    if (g_Renderer) g_Renderer->renderFrame();
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_NativeEngine_nativeTouchEvent(JNIEnv* env, jclass clazz, jfloat dx, jfloat dy) {
    if (g_Renderer) g_Renderer->setCameraLook(dx, dy);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_NativeEngine_nativeGameControl(JNIEnv* env, jclass clazz, jint buttonId, jboolean state) {
    if (g_Renderer) g_Renderer->updateMoveState(buttonId, state == JNI_TRUE);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_NativeEngine_nativeShutdown(JNIEnv* env, jclass clazz) {
    if (g_Renderer) { delete g_Renderer; g_Renderer = nullptr; }
    for (auto loader : g_Archives) { delete loader; }
    g_Archives.clear();
    LOGI("Native Engine Pipeline Terminated Safely.");
}
