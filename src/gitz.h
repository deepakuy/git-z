#pragma once

#include <string>

namespace gitz
{
    // Forward declarations
    class BlobStore;
    class TreeStore;
    class CommitStore;
    class Index;

    class Repository
    {
    private:
        std::string repoPath;
        BlobStore* blobStore;
        TreeStore* treeStore;
        CommitStore* commitStore;
        Index* index;
        std::string headCommitHash;

    public:
        explicit Repository(const std::string& path);
        ~Repository();
        bool init();
        bool open();
        std::string getRepoPath() const;
        Index* getIndex();
        bool stageFile(const std::string& filepath);
        std::string getHeadCommit();
        void setHeadCommit(const std::string& hash);
        bool commit(const std::string& message);
    };
}