#include <jni.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <android/log.h>
#include <cmath>
#include <chrono>
#include <algorithm>

#define LOG_TAG "B1942_Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace Refractor1 {

    // --- Math Structures ---
    struct Vector3 {
        float x, y, z;
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
        Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
        Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
    };

    struct Matrix4x4 {
        float m[16];
        Matrix4x4() {
            for(int i=0; i<16; ++i) m[i] = (i%5 == 0) ? 1.0f : 0.0f;
        }
    };

    // --- RFA Archive structures ---
    struct RfaHeader {
        char magic[24]; // "Refractor 2 Archive"
        uint32_t entryCount;
    };

    struct RfaEntryRaw {
        uint32_t nameLength;
        std::string name;
        uint32_t offset;
        uint32_t compressedSize;
        uint32_t originalSize;
    };

    // --- CON Script Parser ---
    class ConScriptParser {
    public:
        std::map<std::string, std::string> variables;

        void parseFile(const std::string& path) {
            std::ifstream file(path);
            if (!file.is_open()) return;

            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == 'r') continue; // Skip "rem"
                
                std::stringstream ss(line);
                std::string cmd, key, val;
                ss >> cmd >> key >> val;
                
                if (cmd == "var") {
                    variables[key] = val;
                }
            }
        }
    };

    // --- Virtual File System ---
    class RefractorVFS {
    private:
        std::string rootPath;
        std::vector<std::string> mountedRfas;

    public:
        RefractorVFS(const std::string& path) : rootPath(path) {
            LOGI("VFS: Mounting root at %s", path.c_str());
        }

        void scanAndMount() {
            std::string archivesDir = rootPath + "/Mods/bf1942/Archives/";
            DIR* dir = opendir(archivesDir.c_str());
            if (dir) {
                struct dirent* entry;
                while ((entry = readdir(dir)) != nullptr) {
                    std::string name = entry->d_name;
                    if (name.find(".rfa") != std::string::npos) {
                        mountedRfas.push_back(archivesDir + name);
                        LOGI("VFS: Mounted archive: %s", name.c_str());
                    }
                }
                closedir(dir);
            } else {
                LOGE("VFS: Failed to open archives directory.");
            }
        }

        bool fileExists(const std::string& relPath) {
            struct stat buffer;
            std::string full = rootPath + "/" + relPath;
            return (stat(full.c_str(), &buffer) == 0);
        }
    };

    // --- Physics & Controller Engine ---
    class PlayerPhysicsController {
    private:
        Vector3 position;
        Vector3 velocity;
        Vector3 rotation; // x=pitch, y=yaw, z=roll
        
        bool isGrounded = true;
        float moveSpeed = 5.5f;
        float jumpForce = 4.8f;
        float gravity = -9.81f;

        // Corrected static initialization for heights
        static constexpr float EYE_HEIGHT_STAND = 1.7f;
        static constexpr float EYE_HEIGHT_CROUCH = 0.9f;
        float currentEyeHeight;

    public:
        PlayerPhysicsController() 
            : position(0.0f, 1.7f, 0.0f), 
              velocity(0.0f, 0.0f, 0.0f), 
              rotation(0.0f, 0.0f, 0.0f),
              currentEyeHeight(EYE_HEIGHT_STAND) {}

        void update(float dt, bool moveFwd, bool moveBack, bool moveLeft, bool moveRight, bool jump, bool crouch) {
            if (dt <= 0.0f) return;

            // Handle Crouching
            currentEyeHeight = crouch ? EYE_HEIGHT_CROUCH : EYE_HEIGHT_STAND;

            // Calculate movement direction based on Yaw (rotation.y)
            float radYaw = rotation.y * (M_PI / 180.0f);
            Vector3 forward(sin(radYaw), 0, cos(radYaw));
            Vector3 right(cos(radYaw), 0, -sin(radYaw));

            Vector3 moveDir(0, 0, 0);
            if (moveFwd) moveDir += forward;
            if (moveBack) moveDir += (forward * -1.0f);
            if (moveLeft) moveDir += (right * -1.0f);
            if (moveRight) moveDir += right;

            // Apply horizontal velocity
            float currentSpeed = crouch ? (moveSpeed * 0.5f) : moveSpeed;
            velocity.x = moveDir.x * currentSpeed;
            velocity.z = moveDir.z * currentSpeed;

            // Handle Gravity and Jumping
            if (!isGrounded) {
                velocity.y += gravity * dt;
            } else {
                velocity.y = 0;
                if (jump) {
                    velocity.y = jumpForce;
                    isGrounded = false;
                }
            }

            // Apply Velocity to Position
            position += (velocity * dt);

            // Simple floor collision (y=0)
            if (position.y < 0.0f) {
                position.y = 0.0f;
                isGrounded = true;
                velocity.y = 0;
            }
        }

        void applyLookDelta(float deltaPitch, float deltaYaw, float dt) {
            if (dt > 0.0f) {
                rotation.y += deltaYaw * 0.15f;
                rotation.x += deltaPitch * 0.15f;

                // Clamp Pitch to prevent camera flipping
                if (rotation.x > 85.0f) rotation.x = 85.0f;
                if (rotation.x < -85.0f) rotation.x = -85.0f;
            }
        }

        Vector3 getPosition() const { return position; }
        Vector3 getEyePosition() const { return Vector3(position.x, position.y + currentEyeHeight, position.z); }
        Vector3 getRotation() const { return rotation; }
    };

    // --- Master Engine Instance ---
    class MasterRefractorEngine {
    private:
        std::unique_ptr<RefractorVFS> vfs;
        std::unique_ptr<PlayerPhysicsController> player;
        bool isInitialized = false;

        // Input states
        bool btn_fwd = false, btn_back = false, btn_left = false, btn_right = false;
        bool btn_jump = false, btn_crouch = false, btn_fire = false;

    public:
        MasterRefractorEngine() {}

        void initialize(const std::string& gamePath) {
            vfs = std::make_unique<RefractorVFS>(gamePath);
            vfs->scanAndMount();
            player = std::make_unique<PlayerPhysicsController>();
            isInitialized = true;
            LOGI("Refractor Engine V2.5 Initialized Successfully.");
        }

        void setInput(int key, bool pressed) {
            switch(key) {
                case 1: btn_fwd = pressed; break;
                case 2: btn_back = pressed; break;
                case 3: btn_left = pressed; break;
                case 4: btn_right = pressed; break;
                case 5: btn_jump = pressed; break;
                case 6: btn_crouch = pressed; break;
                case 7: btn_fire = pressed; break;
            }
        }

        void step(float dt) {
            if (!isInitialized) return;
            player->update(dt, btn_fwd, btn_back, btn_left, btn_right, btn_jump, btn_crouch);
        }

        void look(float dx, float dy, float dt) {
            if (!isInitialized) return;
            player->applyLookDelta(dy, dx, dt);
        }
    };
}

// --- JNI Global Linkage ---
static std::unique_ptr<Refractor1::MasterRefractorEngine> g_Engine;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInitEngine(JNIEnv* env, jobject thiz, jstring game_path) {
    const char* path = env->GetStringUTFChars(game_path, nullptr);
    g_Engine = std::make_unique<Refractor1::MasterRefractorEngine>();
    g_Engine->initialize(std::string(path));
    env->ReleaseStringUTFChars(game_path, path);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeUpdateInput(JNIEnv* env, jobject thiz, jint key_code, jboolean is_pressed) {
    if (g_Engine) {
        g_Engine->setInput(key_code, is_pressed);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeOnUpdate(JNIEnv* env, jobject thiz, jfloat delta_time) {
    if (g_Engine) {
        g_Engine->step(delta_time);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeOnLook(JNIEnv* env, jobject thiz, jfloat dx, jfloat dy, jfloat dt) {
    if (g_Engine) {
        g_Engine->look(dx, dy, dt);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativePlayBink(JNIEnv* env, jobject thiz, jstring movie_path) {
    LOGI("Native: Requesting Bink playback for Intro.");
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeStopBink(JNIEnv* env, jobject thiz) {
    LOGI("Native: Bink playback stopped.");
}
