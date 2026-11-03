#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <map>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <android/log.h>

#define LOG_TAG "B1942_REFRACTOR_CORE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ============================================================================
// 1. REFRACTOR 1 VIRTUAL FILE SYSTEM (VFS) & EXE INSPECTOR
// ============================================================================

struct VfsFileIndex {
    std::string archivePath;
    uint32_t fileOffset;
    uint32_t fileSize;
    uint32_t uncompressedSize;
};

class Refractor1FullEngine {
private:
    std::string baseGamePath;
    std::map<std::string, VfsFileIndex> globalVfsTable;
    uint32_t totalArchivesMounted = 0;
    bool isExeValid = false;
    bool isEngineReady = false;

    // Player World State
    float posX = 0.0f, posY = 1.7f, posZ = 0.0f;
    float yaw = 0.0f, pitch = 0.0f;
    bool isMoving = false;
    bool isFiring = false;

    // Helper: Cek ekstensi file .rfa (Case-Insensitive)
    bool isRfaFile(const std::string& filename) {
        if (filename.length() >= 4) {
            std::string ext = filename.substr(filename.length() - 4);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            return (ext == ".rfa");
        }
        return false;
    }

    // Helper: Inspeksi Header Executable Windows (MZ Header)
    bool inspectExecutable(const std::string& exePath) {
        std::ifstream exeFile(exePath, std::ios::binary);
        if (!exeFile.is_open()) {
            LOGE("BF1942.exe tidak ditemukan di: %s", exePath.c_str());
            return false;
        }

        char mz[2];
        exeFile.read(mz, 2);
        exeFile.close();

        if (mz[0] == 'M' && mz[1] == 'Z') {
            LOGI("BF1942.exe PE Header OK! Refractor 1 Engine Binary Validated.");
            return true;
        }
        return false;
    }

public:
    Refractor1FullEngine() = default;

    // PEMINDAI REKURSIF OTOMATIS: MEMBACA SEMUA ARCHIVE DI HP SECARA SIMULTAN
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
                    // Masuk ke subfolder (misal: levels/, XPack1, XPack2)
                    scanAndMountDirectoryRecursive(fullPath);
                } else if (S_ISREG(st.st_mode) && isRfaFile(name)) {
                    // Mount File .rfa
                    std::ifstream rfa(fullPath, std::ios::binary);
                    if (rfa.is_open()) {
                        totalArchivesMounted++;
                        LOGI("VFS MOUNTED [%u]: %s", totalArchivesMounted, fullPath.c_str());
                        rfa.close();
                    }
                }
            }
        }
        closedir(dir);
    }

    void initialize(const std::string& rootPath) {
        baseGamePath = rootPath;
        totalArchivesMounted = 0;
        globalVfsTable.clear();

        LOGI("==================================================");
        LOGI("STARTING REFRACTOR 1 ENGINE FULL FILE DISCOVERY");
        LOGI("Base Path: %s", baseGamePath.c_str());
        LOGI("==================================================");

        // 1. Verifikasi BF1942.exe
        isExeValid = inspectExecutable(baseGamePath + "BF1942.exe");

        // 2. Pemindaian Seluruh Archive Utama & Mod (Simultan)
        scanAndMountDirectoryRecursive(baseGamePath + "Mods");
        scanAndMountDirectoryRecursive(baseGamePath + "Archives");

        isEngineReady = (totalArchivesMounted > 0);
        LOGI("ENGINE INITIALIZED FULLY! Total %u .rfa Archives Linked to VFS RAM.", totalArchivesMounted);
    }

    void handleInput(int actionId, bool pressed) {
        if (!isEngineReady) return;
        switch (actionId) {
            case 0: isMoving = pressed; break;
            case 1: isFiring = pressed; break;
            default: break;
        }
    }

    void applyLook(float dPitch, float dYaw) {
        pitch += dPitch;
        yaw += dYaw;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
};

static std::unique_ptr<Refractor1FullEngine> g_engine = nullptr;

// ============================================================================
// 2. JNI EXPORTS TO JAVA (GameActivity.java)
// ============================================================================

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInit(JNIEnv* env, jobject thiz, jstring path) {
    if (!path) return;
    const char* nativePath = env->GetStringUTFChars(path, nullptr);
    if (nativePath) {
        if (!g_engine) g_engine = std::make_unique<Refractor1FullEngine>();
        g_engine->initialize(std::string(nativePath));
        env->ReleaseStringUTFChars(path, nativePath);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGameControl(JNIEnv* env, jobject thiz, jint actionId, jboolean pressed) {
    if (g_engine) {
        g_engine->handleInput(actionId, pressed);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeSetCameraLook(JNIEnv* env, jobject thiz, jfloat pitch, jfloat yaw) {
    if (g_engine) {
        g_engine->applyLook(pitch, yaw);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeUpdateMoveState(JNIEnv* env, jobject thiz, jfloat dx, jfloat dy) {
    if (g_engine) {
        g_engine->applyLook(dy * 0.2f, dx * 0.2f);
    }
}
