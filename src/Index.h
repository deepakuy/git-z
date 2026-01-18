#pragma once

#include <string>
#include <vector>

namespace gitz
{
    struct IndexEntry
    {
        std::string path;        // Relative file path
        std::string blobHash;    // SHA-1 hash of file content
        long timestamp;          // When added to index
    };

    class Index
    {
    private:
        std::string indexFilePath;
        std::vector<IndexEntry> indexEntries;

    public:
        explicit Index(const std::string& indexFilePath);

        bool add(const std::string& path, const std::string& blobHash);
        bool remove(const std::string& path);
        bool contains(const std::string& path) const;
        std::vector<IndexEntry> entries() const;
        bool empty() const;
        void clear();
    };
}
