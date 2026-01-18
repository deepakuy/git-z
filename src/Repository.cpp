#include "gitz.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace gitz
{
    Repository::Repository(const std::string& path)
        : repoPath(path)
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

    std::string Repository::getRepoPath() const
    {
        return repoPath;
    }
}
