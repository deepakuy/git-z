#pragma once

#include <string>

namespace gitz
{
    // Forward declarations
    class BlobStore;
    class TreeStore;
    class CommitStore;

    class Repository
    {
    private:
        std::string repoPath;
        BlobStore* blobStore;
        TreeStore* treeStore;
        CommitStore* commitStore;

    public:
        explicit Repository(const std::string& path);
        ~Repository();
        bool init();
        std::string getRepoPath() const;
    };
}