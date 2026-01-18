#pragma once

#include <cstddef>
#include <string>

namespace gitz
{
    struct Blob
    {
        std::string hash;  // SHA-1 hash (40 characters)
        size_t size;       // Size in bytes
    };

    class BlobStore
    {
    private:
        std::string objectsDir;

    public:
        explicit BlobStore(const std::string& objectsDir);

        std::string addBlobFromFile(const std::string& filePath);
        bool readBlob(const std::string& hash, std::string& outContent) const;
        bool blobExists(const std::string& hash) const;
    };
}
