#pragma once

#include <string>
#include <vector>
#include "Branch.h"

namespace gitz
{
    // Forward declarations
    class BlobStore;
    class TreeStore;
    class CommitStore;
    class Index;
    class BranchManager;

    class Repository
    {
    private:
        std::string repoPath;
        BlobStore* blobStore;
        TreeStore* treeStore;
        CommitStore* commitStore;
        Index* index;
        BranchManager* branchManager;
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
        
        // Branch management
        BranchManager* getBranchManager();
        bool createBranch(const std::string& name);
        bool switchBranch(const std::string& name);
        std::vector<std::string> listBranches();
        bool log();
        
        // Additional getters and operations
        CommitStore* getCommitStore();
        bool checkoutCommit(const std::string& commitHash);
    };
}