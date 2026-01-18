#include "Branch.h"
#include "Index.h"
#include "gitz.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

namespace gitz {

BranchManager::BranchManager(Repository* repository)
    : repo(repository)
{
    branchesPath = fs::path(repo->getRepoPath()) / ".gitz" / "refs" / "heads";
    fs::create_directories(branchesPath);
}

fs::path BranchManager::getBranchPath(const std::string& branchName) const
{
    return branchesPath / branchName;
}

bool BranchManager::branchExists(const std::string& name) const
{
    return fs::exists(getBranchPath(name));
}

bool BranchManager::createBranch(const std::string& name)
{
    if (this->branchExists(name)) {
        std::cerr << "Error: Branch '" << name << "' already exists" << std::endl;
        return false;
    }

    std::string commitHash = repo->getHeadCommit();
    if (commitHash.empty()) {
        std::cerr << "Error: No commits yet" << std::endl;
        return false;
    }

    std::ofstream branchFile(this->getBranchPath(name));
    if (!branchFile) {
        std::cerr << "Error: Failed to create branch file" << std::endl;
        return false;
    }

    branchFile << commitHash;
    return true;
}

bool BranchManager::switchBranch(const std::string& name)
{
    if (!this->branchExists(name)) {
        std::cerr << "Error: Branch '" << name << "' does not exist" << std::endl;
        return false;
    }

    std::ifstream branchFile(this->getBranchPath(name));
    if (!branchFile) {
        std::cerr << "Error: Failed to read branch file" << std::endl;
        return false;
    }

    std::string commitHash;
    std::getline(branchFile, commitHash);

    fs::path headPath = fs::path(repo->getRepoPath()) / ".gitz" / "HEAD";
    std::ofstream headFile(headPath);
    if (!headFile) {
        std::cerr << "Error: Failed to update HEAD" << std::endl;
        return false;
    }

    headFile << "refs/heads/" << name << "\n";
    repo->setHeadCommit(commitHash);
    
    // Clear the staging area
    if (auto index = repo->getIndex()) {
        index->clear();
        std::cout << "Switched to branch '" << name << "'. Staging area cleared." << std::endl;
    }

    return true;
}

std::string BranchManager::currentBranch() const
{
    fs::path headPath = fs::path(repo->getRepoPath()) / ".gitz" / "HEAD";
    std::ifstream headFile(headPath);
    if (!headFile) {
        return "";
    }

    std::string ref;
    std::getline(headFile, ref);

    const std::string prefix = "refs/heads/";
    if (ref.find(prefix) == 0) {
        return ref.substr(prefix.length());
    }

    return "";
}

std::vector<std::string> BranchManager::listBranches() const
{
    std::vector<std::string> branches;

    if (!fs::exists(branchesPath)) {
        return branches;
    }

    for (const auto& entry : fs::directory_iterator(branchesPath)) {
        if (entry.is_regular_file()) {
            branches.push_back(entry.path().filename().string());
        }
    }

    return branches;
}

} // namespace gitz
