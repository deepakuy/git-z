#include "gitz.h"
#include "Blob.h"
#include "Tree.h"
#include "Commit.h"
#include "Index.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace gitz
{
    Repository::Repository(const std::string& path)
        : repoPath(path), blobStore(nullptr), treeStore(nullptr), commitStore(nullptr), index(nullptr)
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

            // Create HEAD file
            std::ofstream headFile(gitzDir / "HEAD");
            if (!headFile.is_open()) {
                std::cerr << "Error: Failed to create HEAD file" << std::endl;
                return false;
            }
            headFile << "refs/heads/master\n";
            headFile.close();

            return true;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    bool Repository::open()
    {
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
}
