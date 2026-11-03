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
#include <chrono>
#include <algorithm>
#include <thread>

#define LOG_TAG "B1942_Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace Refractor1 {

    // --- Math Engine ---
    struct Vector3 {
        float x, y, z;
        Vector3(float _x=0, float _y=0, float _z=0) : x(_x), y(_y), z(_z) {}
        Vector3 operator+(const Vector3& v) const { return Vector3(x+v.x, y+v.y, z+v.z); }
        Vector3 operator*(float s) const { return Vector3(x*s, y*s, z*s); }
        Vector3& operator+=(const Vector3& v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
    };

    // --- Recompiled RFA Loader ---
    struct RfaEntry {
        uint32_t offset;
        uint32_t size;
        std::string archive;
    };

    class VFS {
    public:
        std::map<std::string, RfaEntry> index;
        bool isLoaded = false;

        void scan(std::string root) {
            std::string path = root + "/Mods/bf1942/Archives/";
            DIR* dir = opendir(path.c_str());
            if (!dir) return;
            struct dirent* entry;
            while ((entry = readdir(dir))) {
                std::string name = entry->d_name;
                if (name.find(".rfa") != std::string::npos) {
                    mount(path + name);
                }
            }
            closedir(dir);
            isLoaded = true;
            LOGI("Recompiled VFS: %zu files indexed.", index.size());
        }

        void mount(std::string rfa) {
            std::ifstream f(rfa, std::ios::binary);
            if (!f) return;
            f.seekg(24);
            uint32_t count;
            f.read((char*)&count, 4);
            for (uint32_t i=0; i<count; i++) {
                uint32_t nLen; f.read((char*)&nLen, 4);
                std::vector<char> n(nLen); f.read(n.data(), nLen);
                RfaEntry e;
                f.read((char*)&e.offset, 4);
                f.read((char*)&e.size, 4);
                uint32_t dummy; f.read((char*)&dummy, 4);
                e.archive = rfa;
                index[std::string(n.data(), nLen)] = e;
            }
        }
    };

    // --- Physics Core ---
    class Player {
    public:
        Vector3 pos, rot;
        void move(float dt, bool f, bool b, bool l, bool r) {
            float speed = 5.0f * dt;
            float rad = rot.y * 3.14f / 180.0f;
            if (f) { pos.x += sin(rad)*speed; pos.z += cos(rad)*speed; }
            if (b) { pos.x -= sin(rad)*speed; pos.z -= cos(rad)*speed; }
        }
    };
}

static Refractor1::VFS g_Vfs;
static Refractor1::Player g_Player;
static bool inputs[12] = {false};

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInitEngine(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    std::string p = std::string(cpath);
    // Jalankan scan di thread berbeda agar tidak Frame Drop
    std::thread([p]() { g_Vfs.scan(p); }).detach();
    env->ReleaseStringUTFChars(path, cpath);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeUpdateInput(JNIEnv* env, jobject thiz, jint k, jboolean p) {
    if (k >= 0 && k < 12) inputs[k] = p;
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeOnUpdate(JNIEnv* env, jobject thiz, jfloat dt) {
    g_Player.move(dt, inputs[1], inputs[2], inputs[3], inputs[4]);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeOnLook(JNIEnv* env, jobject thiz, jfloat dx, jfloat dy, jfloat dt) {
    g_Player.rot.y += dx * 0.2f;
}
