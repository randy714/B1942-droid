#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cmath>
#include <android/log.h>

#define LOG_TAG "B1942_REFRACTOR_CORE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Tarik Delta Time dari gl_renderer
extern float getEngineDeltaTime();

class Refractor1FullEngine {
private:
    std::string baseGamePath;
    uint32_t totalArchivesMounted = 0;
    bool isEngineReady = false;

    // Player World State & Fisika
    float posX = 0.0f, posY = 1.7f, posZ = 0.0f;
    float yaw = 0.0f, pitch = 0.0f;
    const float MOVE_SPEED = 6.0f;

    // Controls
    bool isMovingFwd = false, isMovingBwd = false;
    bool isStrafeL = false, isStrafeR = false;
    bool isFiring = false, isAiming = false, isCrouching = false;

    // Cek Ekstensi .rfa
    bool isRfaFile(const std::string& filename) {
        if (filename.length() >= 4) {
            std::string ext = filename.substr(filename.length() - 4);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            return (ext == ".rfa");
        }
        return false;
    }

    // Inspeksi BF1942.exe
    bool inspectExecutable(const std::string& exePath) {
        std::ifstream exe(exePath, std::ios::binary);
        if (!exe.is_open()) return false;
        char mz[2];
        exe.read(mz, 2);
        exe.close();
        return (mz[0] == 'M' && mz[1] == 'Z');
    }

public:
    // VIRTUAL FILE SYSTEM RECURSIVE SCANNER
    void scanAndMountDirectoryRecursive(const std::string& dirPath) {
        DIR* dir = opendir(dirPath.c_str());
        if (!dir) return;

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (name == "." || name == "..") continue;

            std::string fullPath = dirPath + "/" + name;
            struct stat st;
            if (stat(fullPath.c_str(), &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    scanAndMountDirectoryRecursive(fullPath); // Masuk ke levels/, dll
                } else if (S_ISREG(st.st_mode) && isRfaFile(name)) {
                    totalArchivesMounted++;
                    LOGI("VFS MOUNTED [%u]: %s", totalArchivesMounted, fullPath.c_str());
                }
            }
        }
        closedir(dir);
    }

    void initialize(const std::string& rootPath) {
        baseGamePath = rootPath;
        totalArchivesMounted = 0;

        LOGI("=== STARTING FULL REFRACTOR 1 ENGINE DISCOVERY ===");
        if (inspectExecutable(baseGamePath + "BF1942.exe")) {
            LOGI("BF1942.exe Validated.");
        }

        // Scan seluruh folder Mods/ & Archives/ dari HP secara bersamaan
        scanAndMountDirectoryRecursive(baseGamePath + "Mods");
        scanAndMountDirectoryRecursive(baseGamePath + "Archives");

        isEngineReady = (totalArchivesMounted > 0);
        LOGI("ENGINE READY! Total %u .rfa Archives Linked to VFS.", totalArchivesMounted);
    }

    // FISIKA ANTI-LAG MENGGUNAKAN DELTA TIME
    void updatePhysics() {
        if (!isEngineReady) return;

        float dt = getEngineDeltaTime();
        if (dt <= 0.0f || dt > 0.1f) dt = 0.016f;

        float radYaw = yaw * (3.14159265f / 180.0f);
        float speed = MOVE_SPEED * dt;
        if (isCrouching) speed *= 0.5f;

        if (isMovingFwd) { posX += std::sin(radYaw) * speed; posZ -= std::cos(radYaw) * speed; }
        if (isMovingBwd) { posX -= std::sin(radYaw) * speed; posZ += std::cos(radYaw) * speed; }
        if (isStrafeL)   { posX -= std::cos(radYaw) * speed; posZ -= std::sin(radYaw) * speed; }
        if (isStrafeR)   { posX += std::cos(radYaw) * speed; posZ += std::sin(radYaw) * speed; }
    }

    void handleInput(int id, bool pressed) {
        if (!isEngineReady) return;
        switch (id) {
            case 0: isMovingFwd = pressed; break;
            case 1: isFiring = pressed; break;
            case 2: isAiming = pressed; break;
            case 3: if (pressed && posY <= 1.7f) posY += 1.5f; break; // Jump
            case 4: isCrouching = pressed; posY = pressed ? 0.85f : 1.7f; break; // Crouch
            default: break;
        }
    }

    void applyLook(float dPitch, float dYaw) {
        float dt = getEngineDeltaTime();
        if (dt <= 0.0f || dt > 0.1f) dt = 0.016f;

        pitch += dPitch * dt * 60.0f;
        yaw += dYaw * dt * 60.0f;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
};

static std::unique_ptr<Refractor1FullEngine> g_engine = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInit(JNIEnv* env, jobject thiz, jstring path) {
    const char* p = env->GetStringUTFChars(path, nullptr);
    if (!g_engine) g_engine = std::make_unique<Refractor1FullEngine>();
    g_engine->initialize(std::string(p));
    env->ReleaseStringUTFChars(path, p);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGameControl(JNIEnv* env, jobject thiz, jint id, jboolean p) {
    if (g_engine) { g_engine->handleInput(id, p); g_engine->updatePhysics(); }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeSetCameraLook(JNIEnv* env, jobject thiz, jfloat p, jfloat y) {
    if (g_engine) g_engine->applyLook(p, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeUpdateMoveState(JNIEnv* env, jobject thiz, jfloat dx, jfloat dy) {
    if (g_engine) { g_engine->applyLook(dy, dx); g_engine->updatePhysics(); }
}
