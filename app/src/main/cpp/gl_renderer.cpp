#include <jni.h>
#include <GLES3/gl3.h>
#include <android/log.h>

#define LOG_TAG "B1942_RENDERER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class GLRenderer {
public:
    float cameraPitch = 0.0f;
    float cameraYaw = 0.0f;

    void init() {
        glClearColor(0.05f, 0.04f, 0.03f, 1.0f); // Dark Espresso #0D0B0A
        glEnable(GL_DEPTH_TEST);
        LOGI("GLES3 Renderer Initialized.");
    }

    void resize(int w, int h) {
        glViewport(0, 0, w, h);
    }

    void render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void setCameraLook(float pitch, float yaw) {
        cameraPitch += pitch;
        cameraYaw += yaw;
    }

    void updateMoveState(float dx, float dy) {
        // Handle movement vector logic
    }
};

static GLRenderer g_renderer;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGLInit(JNIEnv* env, jobject thiz) {
    g_renderer.init();
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGLResize(JNIEnv* env, jobject thiz, jint width, jint height) {
    g_renderer.resize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGLRender(JNIEnv* env, jobject thiz) {
    g_renderer.render();
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeSetCameraLook(JNIEnv* env, jobject thiz, jfloat pitch, jfloat yaw) {
    g_renderer.setCameraLook(pitch, yaw);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeUpdateMoveState(JNIEnv* env, jobject thiz, jfloat dx, jfloat dy) {
    g_renderer.updateMoveState(dx, dy);
}
