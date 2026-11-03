#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <map>
#include <unordered_map>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <chrono>
#include <android/log.h>

#define LOG_TAG "B1942_REFRACTOR_CORE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

// External Bridge ke gl_renderer.cpp untuk Sinkronisasi Delta Time (Anti Lag)
extern float getEngineDeltaTime();

namespace Refractor1 {

    // ============================================================================
    // 1. REFRACTOR 1 ENGINE BINARY STRUCTURES & FILE HEADERS
    // ============================================================================

    #pragma pack(push, 1)
    struct RfaHeader {
        char magicTag[28];        // Header Identifier "Refractor2 Flat Archive" / "Refractor1"
        uint32_t version;         // Format Version (1/2)
        uint32_t fileTableOffset; // Offset lokasi tabel file internal
        uint32_t entryCount;      // Total file di dalam archive
    };

    struct RfaEntryRaw {
        uint32_t nameLength;
        uint32_t offset;
        uint32_t compressedSize;
        uint32_t uncompressedSize;
    };

    struct StandardMeshHeader {
        uint32_t version;
        uint32_t numLods;
        uint32_t numSubMeshes;
    };

    struct TextureDdsHeader {
        uint32_t dwMagic;
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwHeight;
        uint32_t dwWidth;
        uint32_t dwPitchOrLinearSize;
        uint32_t dwDepth;
        uint32_t dwMipMapCount;
    };
    #pragma pack(pop)

    struct VfsFileNode {
        std::string fileName;
        std::string archivePath;
        uint32_t offset;
        uint32_t compressedSize;
        uint32_t uncompressedSize;
        bool isCompressed;
    };

    // ============================================================================
    // 2. VECTOR & MATRIX 3D MATHEMATICS CORE LIBRARY
    // ============================================================================

    struct Vector3 {
        float x, y, z;

        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        Vector3 operator+(const Vector3& rhs) const { 
            return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); 
        }

        Vector3 operator-(const Vector3& rhs) const { 
            return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); 
        }

        Vector3 operator*(float scalar) const { 
            return Vector3(x * scalar, y * scalar, z * scalar); 
        }

        float length() const { 
            return std::sqrt(x * x + y * y + z * z); 
        }
        
        Vector3 normalize() const {
            float len = length();
            if (len > 0.0001f) {
                return Vector3(x / len, y / len, z / len);
            }
            return Vector3(0.0f, 0.0f, 0.0f);
        }

        static float dot(const Vector3& a, const Vector3& b) {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        static Vector3 cross(const Vector3& a, const Vector3& b) {
            return Vector3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            );
        }
    };

    class Matrix4x4 {
    public:
        float m[16];

        Matrix4x4() {
            setIdentity();
        }

        void setIdentity() {
            std::memset(m, 0, sizeof(m));
            m[0] = 1.0f; 
            m[5] = 1.0f; 
            m[10] = 1.0f; 
            m[15] = 1.0f;
        }

        static Matrix4x4 createPerspective(float fovDeg, float aspect, float nearZ, float farZ) {
            Matrix4x4 res;
            float fovRad = fovDeg * (3.14159265f / 180.0f);
            float tanHalfFov = std::tan(fovRad / 2.0f);

            res.m[0] = 1.0f / (aspect * tanHalfFov);
            res.m[5] = 1.0f / tanHalfFov;
            res.m[10] = -(farZ + nearZ) / (farZ - nearZ);
            res.m[11] = -1.0f;
            res.m[14] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
            res.m[15] = 0.0f;
            return res;
        }

        static Matrix4x4 createRotationY(float angleDeg) {
            Matrix4x4 res;
            float rad = angleDeg * (3.14159265f / 180.0f);
            float c = std::cos(rad);
            float s = std::sin(rad);

            res.m[0] = c;   
            res.m[2] = -s;
            res.m[8] = s;   
            res.m[10] = c;
            return res;
        }

        static Matrix4x4 createRotationX(float angleDeg) {
            Matrix4x4 res;
            float rad = angleDeg * (3.14159265f / 180.0f);
            float c = std::cos(rad);
            float s = std::sin(rad);

            res.m[5] = c;   
            res.m[6] = s;
            res.m[9] = -s;  
            res.m[10] = c;
            return res;
        }
    };

    // ============================================================================
    // 3. REFRACTOR .CON SCRIPT LEXER & PARSER ENGINE
    // ============================================================================

    enum class ConTokenType {
        COMMAND,
        STRING_LITERAL,
        NUMBER,
        COMMENT,
        UNKNOWN
    };

    struct ConToken {
        ConTokenType type;
        std::string value;
    };

    class ConScriptParser {
    private:
        std::map<std::string, std::string> scriptVariables;
        std::vector<std::string> loadedLevels;

    public:
        ConScriptParser() = default;

        void tokenizeAndParse(const std::string& scriptContent) {
            std::stringstream stream(scriptContent);
            std::string line;

            while (std::getline(stream, line)) {
                if (line.empty()) continue;

                // Trim leading whitespace
                size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
                if (firstNonSpace == std::string::npos) continue;
                line = line.substr(firstNonSpace);

                // Ignore comments
                if (line.length() >= 2 && (line.substr(0, 2) == "rem" || line.substr(0, 2) == "REM")) continue;
                if (line[0] == '#') continue;

                std::stringstream lineStream(line);
                std::string command, arg1, arg2;
                lineStream >> command >> arg1 >> arg2;

                if (!command.empty()) {
                    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
                    scriptVariables[command] = arg1;

                    if (command == "game.setservergame3dmap" || command == "game.addmod") {
                        loadedLevels.push_back(arg1);
                        LOGI("CON ENGINE: Map/Mod Registered -> %s", arg1.c_str());
                    }
                }
            }
            LOGI("CON Script Tokenizer finished. Configured %zu settings.", scriptVariables.size());
        }

        std::string getVariable(const std::string& key) const {
            auto it = scriptVariables.find(key);
            if (it != scriptVariables.end()) {
                return it->second;
            }
            return "";
        }

        size_t getLoadedLevelCount() const {
            return loadedLevels.size();
        }
    };

    // ============================================================================
    // 4. VIRTUAL FILE SYSTEM (VFS) & RFA ARCHIVE EXTRACTOR
    // ============================================================================

    class RefractorVFS {
    private:
        std::string baseDirectory;
        std::unordered_map<std::string, VfsFileNode> globalFileTable;
        uint32_t mountedArchiveCount;
        uint32_t totalIndexedFiles;

        bool isRfaExtension(const std::string& name) const {
            if (name.length() >= 4) {
                std::string ext = name.substr(name.length() - 4);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                return (ext == ".rfa");
            }
            return false;
        }

        std::string toLowerCase(std::string str) const {
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            return str;
        }

    public:
        RefractorVFS() : mountedArchiveCount(0), totalIndexedFiles(0) {}

        void setBaseDirectory(const std::string& dir) {
            baseDirectory = dir;
        }

        bool parseRfaArchive(const std::string& fullPath) {
            std::ifstream file(fullPath, std::ios::binary);
            if (!file.is_open()) {
                LOGE("VFS Error: Failed to open RFA archive: %s", fullPath.c_str());
                return false;
            }

            RfaHeader header;
            file.read(reinterpret_cast<char*>(&header), sizeof(RfaHeader));

            if (file.gcount() < sizeof(RfaHeader)) {
                file.close();
                return false;
            }

            // Membaca RFA File Entries
            if (header.entryCount > 0 && header.entryCount < 100000) {
                file.seekg(header.fileTableOffset, std::ios::beg);
                
                for (uint32_t i = 0; i < header.entryCount; ++i) {
                    uint32_t nameLen = 0;
                    file.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));
                    
                    if (nameLen > 0 && nameLen < 512) {
                        std::vector<char> nameBuf(nameLen + 1, 0);
                        file.read(nameBuf.data(), nameLen);
                        
                        RfaEntryRaw entryRaw;
                        file.read(reinterpret_cast<char*>(&entryRaw), sizeof(RfaEntryRaw));

                        VfsFileNode node;
                        node.fileName = toLowerCase(std::string(nameBuf.data()));
                        node.archivePath = fullPath;
                        node.offset = entryRaw.offset;
                        node.compressedSize = entryRaw.compressedSize;
                        node.uncompressedSize = entryRaw.uncompressedSize;
                        node.isCompressed = (entryRaw.compressedSize != entryRaw.uncompressedSize);

                        globalFileTable[node.fileName] = node;
                        totalIndexedFiles++;
                    } else {
                        break;
                    }
                }
            }

            mountedArchiveCount++;
            LOGI("VFS MOUNTED [%u]: %s (Indexed Entries: %u)", mountedArchiveCount, fullPath.c_str(), header.entryCount);
            file.close();
            return true;
        }

        void scanDirectoryRecursive(const std::string& currentPath) {
            DIR* dir = opendir(currentPath.c_str());
            if (!dir) return;

            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string name = entry->d_name;
                if (name == "." || name == "..") continue;

                std::string fullPath = currentPath + "/" + name;
                struct stat st;

                if (stat(fullPath.c_str(), &st) == 0) {
                    if (S_ISDIR(st.st_mode)) {
                        // Scan subfolder secara rekursif (levels/, XPack1, XPack2, dll)
                        scanDirectoryRecursive(fullPath);
                    } else if (S_ISREG(st.st_mode) && isRfaExtension(name)) {
                        parseRfaArchive(fullPath);
                    }
                }
            }
            closedir(dir);
        }

        bool hasFile(const std::string& fileName) const {
            std::string key = toLowerCase(fileName);
            return globalFileTable.find(key) != globalFileTable.end();
        }

        uint32_t getMountedArchiveCount() const {
            return mountedArchiveCount;
        }

        uint32_t getTotalIndexedFiles() const {
            return totalIndexedFiles;
        }
    };

    // ============================================================================
    // 5. PLAYER PHYSICS & 10 TOUCH CONTROL STATE MACHINE
    // ============================================================================

    class PlayerPhysicsController {
    private:
        Vector3 position;
        Vector3 velocity;
        float yawDeg;
        float pitchDeg;

        // Status 10 Tombol Kontrol Touch
        bool moveForward;
        bool moveBackward;
        bool strafeLeft;
        bool strafeRight;
        bool isCrouching;
        bool isAiming;
        bool isFiring;
        bool inVehicle;

        const float EYE_HEIGHT_STAND = 1.70f;
        const float EYE_HEIGHT_CROUCH = 0.85f;
        const float MOVE_SPEED = 6.0f; // 6.0 meter per detik

    public:
        PlayerPhysicsController() 
            : position(0.0f, EYE_HEIGHT_STAND, 0.0f),
              velocity(0.0f, 0.0f, 0.0f),
              yawDeg(0.0f), pitchDeg(0.0f),
              moveForward(false), moveBackward(false),
              strafeLeft(false), strafeRight(false),
              isCrouching(false), isAiming(false),
              isFiring(false), inVehicle(false) {}

        void updatePhysics(float dt) {
            if (dt <= 0.0f || dt > 0.1f) dt = 0.016f; // Clamp Delta Time

            float radYaw = yawDeg * (3.14159265f / 180.0f);
            float currentSpeed = MOVE_SPEED * dt;

            if (isCrouching) {
                currentSpeed *= 0.5f;
                position.y = EYE_HEIGHT_CROUCH;
            } else {
                position.y = EYE_HEIGHT_STAND;
            }

            Vector3 forwardDir(std::sin(radYaw), 0.0f, -std::cos(radYaw));
            Vector3 rightDir(std::cos(radYaw), 0.0f, std::sin(radYaw));

            Vector3 moveVector(0.0f, 0.0f, 0.0f);

            if (moveForward)  moveVector = moveVector + forwardDir;
            if (moveBackward) moveVector = moveVector - forwardDir;
            if (strafeRight)  moveVector = moveVector + rightDir;
            if (strafeLeft)   moveVector = moveVector - rightDir;

            if (moveVector.length() > 0.001f) {
                moveVector = moveVector.normalize();
                position = position + (moveVector * currentSpeed);
            }
        }

        void handleTouchButton(int actionId, bool isPressed) {
            switch (actionId) {
                case 0: moveForward = isPressed; break;                        // MOVE
                case 1: isFiring = isPressed; if(isPressed) LOGI("FIRE!"); break;  // FIRE
                case 2: isAiming = isPressed; break;                           // AIM
                case 3:                                                        // JUMP
                    if (isPressed && position.y <= EYE_HEIGHT_STAND) {
                        position.y += 1.25f;
                    }
                    break;
                case 4: isCrouching = isPressed; break;                        // CROUCH
                case 5: if (isPressed) LOGI("RELOAD WEAPON"); break;          // RELOAD
                case 6: if (isPressed) LOGI("SWITCH WEAPON"); break;          // WEAPON
                case 7:                                                        // VEHICLE
                    if (isPressed) {
                        inVehicle = !inVehicle;
                        LOGI("VEHICLE STATE TOGGLED: %s", inVehicle ? "IN VEHICLE" : "ON FOOT");
                    }
                    break;
                case 8: if (isPressed) LOGI("SHOW SCOREBOARD"); break;         // SCORE
                case 9: if (isPressed) LOGI("OPEN PAUSE MENU"); break;         // MENU
                default: break;
            }
        }

        void applyLookDelta(float deltaPitch, float deltaYaw, float dt) {
            if (dt 
