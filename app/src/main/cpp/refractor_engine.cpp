#include <jni.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <android/log.h>

#define LOG_TAG "B1942_Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace Refractor1 {
    struct RfaEntry {
        uint32_t offset;
        uint32_t size;
    };

    class MasterEngine {
    public:
        std::map<std::string, RfaEntry> fileIndex;
        std::string activeRfa;

        void loadRfaIndex(const std::string& rfaPath) {
            std::ifstream file(rfaPath, std::ios::binary);
            if (!file.is_open()) return;

            activeRfa = rfaPath;
            file.seekg(24); // Lewati header
            uint32_t count;
            file.read((char*)&count, 4);

            for (uint32_t i = 0; i < count; i++) {
                uint32_t nameLen;
                file.read((char*)&nameLen, 4);
                std::vector<char> name(nameLen);
                file.read(name.data(), nameLen);
                
                RfaEntry e;
                file.read((char*)&e.offset, 4);
                file.read((char*)&e.size, 4);
                uint32_t dummy; file.read((char*)&dummy, 4); // compressed size

                fileIndex[std::string(name.data(), nameLen)] = e;
            }
            LOGI("Engine: Terindeks %d file dari %s", count, rfaPath.c_str());
        }

        std::vector<char> getFileData(const std::string& name) {
            if (fileIndex.find(name) == fileIndex.end()) return {};
            RfaEntry e = fileIndex[name];
            std::ifstream file(activeRfa, std::ios::binary);
            file.seekg(e.offset);
            std::vector<char> data(e.size);
            file.read(data.data(), e.size);
            return data;
        }
    };
}

static Refractor1::MasterEngine g_Engine;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInitEngine(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    std::string base = std::string(cpath);
    // Langsung target menu.rfa untuk menampilkan aset
    g_Engine.loadRfaIndex(base + "/Mods/bf1942/Archives/menu.rfa");
    env->ReleaseStringUTFChars(path, cpath);
}

// JNI untuk Renderer mengambil data gambar
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_b1942_engine_GameActivity_nativeGetAssetData(JNIEnv* env, jobject thiz, jstring name) {
    const char* cname = env->GetStringUTFChars(name, nullptr);
    std::vector<char> data = g_Engine.getFileData(std::string(cname));
    env->ReleaseStringUTFChars(name, cname);
    
    if (data.empty()) return nullptr;
    jbyteArray arr = env->NewByteArray(data.size());
    env->SetByteArrayRegion(arr, 0, data.size(), (jbyte*)data.data());
    return arr;
}
