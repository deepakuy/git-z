#include "gitz.h"
#include "Blob.h"
#include "Tree.h"
#include "Commit.h"
#include "Index.h"
#include "Branch.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace gitz
{
    Repository::Repository(const std::string& path)
        : repoPath(path), blobStore(nullptr), treeStore(nullptr), commitStore(nullptr), 
          index(nullptr), branchManager(nullptr), headCommitHash(""),
          blob_cache(nullptr), commit_cache(nullptr), history_cache(nullptr)
    {
    }

    bool Repository::init()
    {
        fs::path gitzDir = fs::path(repoPath) / ".gitz";

        if (fs::exists(gitzDir)) {
            std::cerr << "Error: Repository already initialized at " << repoPath << std::endl;
            return false;
        }

        try {
            // Create directory structure
            fs::create_directories(gitzDir / "objects");
            fs::create_directories(gitzDir / "refs" / "heads");

            // Create BlobStore
            blobStore = new BlobStore(repoPath + "/.gitz/objects");

            // Create TreeStore
            treeStore = new TreeStore(repoPath + "/.gitz/objects");

            // Create CommitStore
            commitStore = new CommitStore(repoPath + "/.gitz/objects");

            // Create Index
            index = new Index(repoPath + "/.gitz/index");

            // Create BranchManager
            branchManager = new BranchManager(this);

            // Initialize caches
            blob_cache = std::make_unique<BlobCache>(100);
            commit_cache = std::make_unique<CommitCache>(50);
            history_cache = std::make_unique<HistoryCache>();

            // Create HEAD file
            std::ofstream headFile(gitzDir / "HEAD");
            if (!headFile.is_open()) {
                std::cerr << "Error: Failed to create HEAD file" << std::endl;
                return false;
            }
            headFile << "refs/heads/master\n";
            headFile.close();

            // Create HEAD_COMMIT file
            std::ofstream headCommitFile(gitzDir / "HEAD_COMMIT");
            if (!headCommitFile.is_open()) {
                std::cerr << "Error: Failed to create HEAD_COMMIT file" << std::endl;
                return false;
            }
            headCommitFile.close();

            return true;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    bool Repository::open()
    {
        // If already opened, return success
        if (blobStore && treeStore && commitStore && index) {
            if (!blob_cache)
                blob_cache = std::make_unique<BlobCache>(100);
            if (!commit_cache)
                commit_cache = std::make_unique<CommitCache>(50);
            if (!history_cache)
                history_cache = std::make_unique<HistoryCache>();
            return true;
        }

        fs::path gitzDir = fs::path(repoPath) / ".gitz";

        // Check if .gitz directory exists
        if (!fs::exists(gitzDir)) {
            std::cerr << "Error: No repository found at " << repoPath << std::endl;
            return false;
        }

        try {
            // Initialize stores without creating directories
            blobStore = new BlobStore(repoPath + "/.gitz/objects");
            treeStore = new TreeStore(repoPath + "/.gitz/objects");
            commitStore = new CommitStore(repoPath + "/.gitz/objects");
            index = new Index(repoPath + "/.gitz/index");
            branchManager = new BranchManager(this);

            // Initialize caches if not already initialized
            if (!blob_cache)
                blob_cache = std::make_unique<BlobCache>(100);
            if (!commit_cache)
                commit_cache = std::make_unique<CommitCache>(50);
            if (!history_cache)
                history_cache = std::make_unique<HistoryCache>();

            // Read HEAD_COMMIT if it exists
            fs::path headCommitPath = gitzDir / "HEAD_COMMIT";
            if (fs::exists(headCommitPath)) {
                std::ifstream headCommitFile(headCommitPath);
                if (headCommitFile.is_open()) {
                    std::getline(headCommitFile, headCommitHash);
                    headCommitFile.close();
                }
            }

            return true;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    Repository::~Repository()
    {
        if (blobStore != nullptr) {
            delete blobStore;
        }
        if (treeStore != nullptr) {
            delete treeStore;
        }
        if (commitStore != nullptr) {
            delete commitStore;
        }
        if (index != nullptr) {
            delete index;
        }
        if (branchManager != nullptr) {
            delete branchManager;
        }
    }

    std::string Repository::getRepoPath() const
    {
        return repoPath;
    }

    Index* Repository::getIndex()
    {
        return index;
    }

    bool Repository::stageFile(const std::string& filepath)
    {
        // Construct full path
        fs::path fullPath = fs::path(repoPath) / filepath;

        // Check if file exists
        if (!fs::exists(fullPath)) {
            std::cerr << "Error: File not found: " << fullPath << std::endl;
            return false;
        }

        // Create blob from file
        if (!blobStore) {
            std::cerr << "Error: BlobStore not initialized" << std::endl;
            return false;
        }
        std::string blobHash = blobStore->addBlobFromFile(fullPath.string());
        if (blobHash.empty()) {
            return false;
        }

        // Add entry to index
        if (!index) {
            std::cerr << "Error: Index not initialized" << std::endl;
            return false;
        }
        index->add(filepath, blobHash);
        return true;
    }

    std::string Repository::getHeadCommit()
    {
        return headCommitHash;
    }

    void Repository::setHeadCommit(const std::string& hash)
    {
        headCommitHash = hash;

        // Write to HEAD_COMMIT file
        fs::path gitzDir = fs::path(repoPath) / ".gitz";
        fs::path headCommitPath = gitzDir / "HEAD_COMMIT";

        try {
            std::ofstream headCommitFile(headCommitPath);
            if (headCommitFile.is_open()) {
                headCommitFile << hash;
                headCommitFile.close();
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    bool Repository::commit(const std::string& message)
    {
        if (!index || index->empty()) {
            std::cerr << "Error: No files staged for commit." << std::endl;
            return false;
        }

        if (!treeStore || !commitStore) {
            std::cerr << "Error: Repository not properly initialized." << std::endl;
            return false;
        }

        std::vector<TreeEntry> treeEntries;

        for (const auto& entry : index->entries()) {
            TreeEntry te;
            te.name = entry.path;
            te.hash = entry.blobHash;
            te.isDirectory = false;
            treeEntries.push_back(te);
        }

        std::string treeHash = treeStore->createTree(treeEntries);
        if (treeHash.empty()) {
            std::cerr << "Error: Failed to create tree object." << std::endl;
            return false;
        }

        std::string parentHash = getHeadCommit();

        // Validate parent commit exists if there is one
        if (!parentHash.empty() && !commitStore->commitExists(parentHash)) {
            std::cerr << "Error: HEAD points to invalid commit." << std::endl;
            return false;
        }

        std::string commitHash =
            commitStore->createCommit(treeHash, parentHash, message);

        if (commitHash.empty()) {
            std::cerr << "Error: Failed to create commit." << std::endl;
            return false;
        }

        setHeadCommit(commitHash);
        index->clear();

        std::cout << "Committed as " << commitHash << std::endl;
        return true;
    }

    BranchManager* Repository::getBranchManager() 
    {
        return branchManager;
    }

    bool Repository::createBranch(const std::string& name) 
    {
        if (!branchManager) {
            std::cerr << "Error: BranchManager not initialized" << std::endl;
            return false;
        }
        return branchManager->createBranch(name);
    }

    bool Repository::switchBranch(const std::string& name) 
    {
        if (!branchManager) {
            std::cerr << "Error: BranchManager not initialized" << std::endl;
            return false;
        }
        return branchManager->switchBranch(name);
    }

    std::vector<std::string> Repository::listBranches() 
    {
        if (!branchManager) {
            return {};
        }
        return branchManager->listBranches();
    }

    bool Repository::log()
    {
        if (headCommitHash.empty()) {
            std::cout << "No commits yet" << std::endl;
            return true;
        }

        std::string currentHash = headCommitHash;
        while (!currentHash.empty()) {
            Commit commit;
            if (!commitStore || !commitStore->readCommit(currentHash, commit)) {
                std::cerr << "Error: Failed to read commit " << currentHash << std::endl;
                return false;
            }

            // Print commit details
            std::cout << "commit " << currentHash << std::endl;
            std::cout << "Author: " << commit.author << std::endl;
            std::time_t t = static_cast<std::time_t>(commit.timestamp);
            std::cout << "Date:   " << std::ctime(&t);
            std::cout << "\n    " << commit.message << "\n" << std::endl;

            // Move to parent commit
            currentHash = commit.parentHash;
        }

        return true;
    }

    CommitStore* Repository::getCommitStore() {
        return commitStore;
    }

    bool Repository::checkoutCommit(const std::string& commitHash) {
        if (!commitStore) {
            std::cerr << "Error: CommitStore not initialized" << std::endl;
            return false;
        }

        if (!commitStore->commitExists(commitHash)) {
            std::cerr << "Error: Commit does not exist" << std::endl;
            return false;
        }

        // Write commit hash to .gitz/HEAD (detached HEAD state)
        std::filesystem::path headFile = std::filesystem::path(repoPath) / ".gitz" / "HEAD";
        std::ofstream headOut(headFile);
        if (!headOut) {
            std::cerr << "Error: Failed to write HEAD file" << std::endl;
            return false;
        }
        headOut << "detached:" << commitHash << "\n";

        headOut.close();

        // Update internal state
        setHeadCommit(commitHash);

        // Clear staging area
        if (index) {
            index->clear();
        }

        return true;
}

bool Repository::readBlobCached(const std::string& hash, std::string& out_content) {
    // 1. Try cache first
    if (blob_cache && blob_cache->get(hash, out_content)) {
        return true;
    }

    // 2. Safety check
    if (!blobStore) {
        std::cerr << "Error: BlobStore not initialized" << std::endl;
        return false;
    }

    // 3. Read from disk
    if (!blobStore->readBlob(hash, out_content)) {
        return false;
    }

    // 4. Store in cache
    if (blob_cache) {
        blob_cache->put(hash, out_content);
    }

    return true;
}

} // namespace gitz
