#ifndef RFA_LOADER_H
#define RFA_LOADER_H

#include <string>
#include <vector>
#include <cstdint>

struct RFAEntry {
    std::string fileName;
    uint32_t offset;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
};

class RFALoader {
public:
    RFALoader();
    ~RFALoader();
    bool openArchive(const std::string& archivePath);
    bool extractFile(const std::string& entryName, std::vector<uint8_t>& outBuffer);
    void close();

private:
    std::string m_path;
    std::vector<RFAEntry> m_entries;
    bool m_isOpen;
};

#endif // RFA_LOADER_H
