#pragma once

#include <string>

namespace gitz
{
    // Forward declaration
    class BlobStore;

    class Repository
    {
    private:
        std::string repoPath;
        BlobStore* blobStore;

    public:
        explicit Repository(const std::string& path);
        ~Repository();
        bool init();
        std::string getRepoPath() const;
    };
}