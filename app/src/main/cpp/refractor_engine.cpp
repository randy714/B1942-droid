#include <jni.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <android/log.h>
#include <cmath>
#include <algorithm>
#include <cstring>

#define LOG_TAG "B1942_Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace Refractor1 {

    // --- Vector3 Math Engine ---
    struct Vector3 {
        float x, y, z;
        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
        Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
        Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
        Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
        Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
        
        float length() const { return std::sqrt(x * x + y * y + z * z); }
        Vector3 normalized() const {
            float len = length();
            return (len > 0.0001f) ? *this * (1.0f / len) : Vector3(0, 0, 0);
        }
    };

    // --- RFA Archive Data Structure ---
    struct RfaFileEntry {
        std::string archivePath;
        uint32_t offset;
        uint32_t size;
        uint32_t compressedSize;
    };

    // --- Virtual File System (VFS) ---
    class RefractorVFS {
    public:
        std::map<std::string, RfaFileEntry> indexMap;
        std::string gameRoot;

        RefractorVFS(const std::string& path) : gameRoot(path) {}

        void mountArchives() {
            std::string archDir = gameRoot + "/Mods/bf1942/Archives/";
            DIR* dir = opendir(archDir.c_str());
            if (!dir) {
                LOGE("VFS: Folder archives tidak ditemukan di %s", archDir.c_str());
                return;
            }

            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string name = entry->d_name;
                if (name.find(".rfa") != std::string::npos || name.find(".RFA") != std::string::npos) {
                    parseRfaHeader(archDir + name);
                }
            }
            closedir(dir);
            LOGI("VFS: Mount selesai. Total file aset terdeteksi: %size_t", indexMap.size());
        }

        void parseRfaHeader(const std::string& fullPath) {
            std::ifstream file(fullPath, std::ios::binary);
            if (!file.is_open()) return;

            char magic[24];
            file.read(magic, 24);

            uint32_t count = 0;
            file.read(reinterpret_cast<char*>(&count), 4);

            for (uint32_t i = 0; i < count; ++i) {
                uint32_t nameLen = 0;
                file.read(reinterpret_cast<char*>(&nameLen), 4);
                if (nameLen == 0 || nameLen > 512) break;

                std::vector<char> buf(nameLen);
                file.read(buf.data(), nameLen);
                std::string filename(buf.data(), nameLen);

                RfaFileEntry e;
                e.archivePath = fullPath;
                file.read(reinterpret_cast<char*>(&e.offset), 4);
                file.read(reinterpret_cast<char*>(&e.size), 4);
                file.read(reinterpret_cast<char*>(&e.compressedSize), 4);

                std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
                indexMap[filename] = e;
            }
            LOGI("VFS: Mounted archive RFA -> %s", fullPath.c_str());
        }

        bool hasFile(const std::string& relPath) {
            struct stat buffer;
            std::string full = gameRoot + "/" + relPath;
            return (stat(full.c_str(), &buffer) == 0);
        }
    };

    // --- Physics Controller ---
    class PlayerPhysicsController {
    private:
        Vector3 pos;
        Vector3 vel;
        Vector3 rot;
        bool isGrounded;
        float eyeHeight;

    public:
        PlayerPhysicsController() 
            : pos(0.0f, 1.7f, 0.0f), vel(0.0f, 0.0f, 0.0f), rot(0.0f, 0.0f, 0.0f),
              isGrounded(true), eyeHeight(1.7f) {}

        void update(float dt, bool fwd, bool back, bool left, bool right, bool jump, bool crouch) {
            if (dt <= 0.0f) return;

            eyeHeight = crouch ? 0.9f : 1.7f;
            float radYaw = rot.y * (M_PI / 180.0f);

            Vector3 fwdDir(std::sin(radYaw), 0.0f, std::cos(radYaw));
            Vector3 rightDir(std::cos(radYaw), 0.0f, -std::sin(radYaw));

            Vector3 moveDir(0.0f, 0.0f, 0.0f);
            if (fwd) moveDir += fwdDir;
            if (back) moveDir -= fwdDir;
            if (left) moveDir -= rightDir;
            if (right) moveDir += rightDir;

            float speed = crouch ? 2.5f : 5.5f;
            vel.x = moveDir.x * speed;
            vel.z = moveDir.z * speed;

            if (!isGrounded) {
                vel.y -= 9.81f * dt;
            } else {
                vel.y = 0.0f;
                if (jump) {
                    vel.y = 4.8f;
                    isGrounded = false;
                }
            }

            pos += vel * dt;

            if (pos.y < 0.0f) {
                pos.y = 0.0f;
                isGrounded = true;
                vel.y = 0.0f;
            }
        }

        void applyLookDelta(float dx, float dy) {
            rot.y += dx * 0.15f;
            rot.x = std::clamp(rot.x + dy * 0.15f, -85.0f, 85.0f);
        }

        Vector3 getPosition() const { return pos; }
    };

    // --- Refractor Engine Master Core ---
    class MasterRefractorEngine {
    private:
        std::unique_ptr<RefractorVFS> vfs;
        std::unique_ptr<PlayerPhysicsController> player;
        bool touchInputs[12] = {false};
        bool isReady = false;

    public:
        void initialize(const std::string& gamePath) {
            vfs = std::make_unique<RefractorVFS>(gamePath);
            vfs->mountArchives();
            player = std::make_unique<PlayerPhysicsController>();
            isReady = true;
            LOGI("Refractor 1 Mobile Engine Core: READY.");
        }

        void setInputState(int key, bool pressed) {
            if (key >= 0 && key < 12) {
                touchInputs[key] = pressed;
            }
        }

        void updateFrame(float dt) {
            if (!isReady || !player) return;
            player->update(dt, touchInputs[1], touchInputs[2], touchInputs[3], touchInputs[4], touchInputs[5], touchInputs[6]);
        }

        void applyLook(float dx, float dy) {
            if (!isReady || !player) return;
            player->applyLookDelta(dx, dy);
        }
    };
}

// --- JNI Bridge Global Reference ---
static std::unique_ptr<Refractor1::MasterRefractorEngine> g_Engine = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInitEngine(JNIEnv* env, jobject thiz, jstring game_path) {
    const char* pathStr = env->GetStringUTFChars(game_path, nullptr);
    g_Engine = std::make_unique<Refractor1::MasterRefractorEngine>();
    g_Engine->initialize(std::string(pathStr));
    env->ReleaseStringUTFChars(game_path, pathStr);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeUpdateInput(JNIEnv* env, jobject thiz, jint key_code, jboolean is_pressed) {
    if (g_Engine) g_Engine->setInputState(key_code, is_pressed);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeOnUpdate(JNIEnv* env, jobject thiz, jfloat delta_time) {
    if (g_Engine) g_Engine->updateFrame(delta_time);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeOnLook(JNIEnv* env, jobject thiz, jfloat dx, jfloat dy, jfloat dt) {
    if (g_Engine) g_Engine->applyLook(dx, dy);
}
