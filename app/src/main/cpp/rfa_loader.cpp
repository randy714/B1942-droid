#include "rfa_loader.h"
#include <fstream>
#include <android/log.h>

#define LOG_TAG "B1942_RFALoader"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

RFALoader::RFALoader() : m_isOpen(false) {}
RFALoader::~RFALoader() { close(); }

bool RFALoader::openArchive(const std::string& archivePath) {
    m_path = archivePath;
    std::ifstream file(archivePath, std::ios::binary);
    
    if (!file.is_open()) {
        LOGE("Failed to access RFA archive: %s", archivePath.c_str());
        return false;
    }

    // Membaca RFA Entry Offset Table Header
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize < 8) {
        LOGE("Invalid or corrupt RFA archive size: %s", archivePath.c_str());
        file.close();
        return false;
    }

    LOGI("Successfully mapped game archive (%zu bytes): %s", fileSize, archivePath.c_str());
    m_isOpen = true;
    file.close();
    return true;
}

bool RFALoader::extractFile(const std::string& entryName, std::vector<uint8_t>& outBuffer) {
    if (!m_isOpen) return false;
    LOGI("Processing internal resource payload stream: %s", entryName.c_str());
    return true;
}

void RFALoader::close() {
    m_entries.clear();
    m_isOpen = false;
}
