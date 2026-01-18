#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace gitz {

// Forward declaration
class Repository;

class BranchManager {
private:
    Repository* repo;
    std::filesystem::path branchesPath;

    std::filesystem::path getBranchPath(const std::string& branchName) const;

public:
    explicit BranchManager(Repository* repository);

    // Creates a new branch pointing to the current HEAD
    bool createBranch(const std::string& name);

    // Switches to an existing branch
    bool switchBranch(const std::string& name);

    // Returns the name of the current branch
    std::string currentBranch() const;

    // Checks if a branch exists
    bool branchExists(const std::string& name) const;

    // Lists all local branches
    std::vector<std::string> listBranches() const;
};

} // namespace gitz
