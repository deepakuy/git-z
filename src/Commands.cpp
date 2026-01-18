#include "Commands.h"
#include "Repository.h"
#include "Index.h"
#include "Branch.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace gitz {

bool Commands::init(Repository* repo, const std::string& path) {
    if (!repo) {
        std::cerr << "Error: Repository pointer is null" << std::endl;
        return false;
    }

    if (!Validator::validateNoNestedRepo(path)) {
        std::cerr << "Error: " << Validator::getValidationError() << std::endl;
        return false;
    }

    if (!repo->init()) {
        std::cerr << "Error: Failed to initialize repository" << std::endl;
        return false;
    }

    std::cout << "Initialized empty Git-Z repository in " << fs::absolute(path) << "/.gitz/" << std::endl;
    return true;
}

bool Commands::add(Repository* repo, const std::string& filepath) {
    if (!repo) {
        std::cerr << "Error: Repository pointer is null" << std::endl;
        return false;
    }

    if (!repo->open()) {
        std::cerr << "Error: Could not open repository" << std::endl;
        return false;
    }

    if (!Validator::validateFileExists(filepath)) {
        std::cerr << "Error: " << Validator::getValidationError() << std::endl;
        return false;
    }

    if (!repo->stageFile(filepath)) {
        std::cerr << "Error: Failed to stage file " << filepath << std::endl;
        return false;
    }

    std::cout << "Added " << filepath << " to staging area" << std::endl;
    return true;
}

bool Commands::commit(Repository* repo, const std::string& message) {
    if (!repo) {
        std::cerr << "Error: Repository pointer is null" << std::endl;
        return false;
    }

    if (!repo->open()) {
        std::cerr << "Error: Could not open repository" << std::endl;
        return false;
    }

    if (!Validator::validateCommitMessage(message)) {
        std::cerr << "Error: " << Validator::getValidationError() << std::endl;
        return false;
    }

    Index* index = repo->getIndex();
    if (!Validator::validateStagingAreaNotEmpty(index)) {
        std::cerr << "Error: " << Validator::getValidationError() << std::endl;
        return false;
    }

    if (!repo->commit(message)) {
        std::cerr << "Error: Failed to create commit" << std::endl;
        return false;
    }

    std::cout << "Created new commit" << std::endl;
    return true;
}

void Commands::status(Repository* repo) {
    if (!repo) {
        std::cerr << "Error: Repository pointer is null" << std::endl;
        return;
    }

    if (!repo->open()) {
        std::cerr << "Error: Could not open repository" << std::endl;
        return;
    }

    // Get current branch name
    std::string branch = "unknown";
    BranchManager* bm = repo->getBranchManager();
    if (bm) {
        branch = bm->currentBranch();
    }

    std::cout << "On branch " << branch << std::endl;

    // Check for staged changes
    Index* index = repo->getIndex();
    if (!index || index->empty()) {
        std::cout << "nothing to commit, working tree clean" << std::endl;
    } else {
        std::cout << "Changes to be committed:" << std::endl;
        for (const auto& entry : index->entries()) {
            std::cout << "  new file: " << entry.path << std::endl;
        }
    }
}

bool Commands::branch(Repository* repo, const std::string& branch_name) {
    if (!repo) {
        std::cerr << "Error: Repository pointer is null" << std::endl;
        return false;
    }

    if (!repo->open()) {
        std::cerr << "Error: Could not open repository" << std::endl;
        return false;
    }

    // If branch_name is empty, list branches
    if (branch_name.empty()) {
        BranchManager* bm = repo->getBranchManager();
        std::string current = bm ? bm->currentBranch() : "unknown";
        
        for (const auto& branch : repo->listBranches()) {
            if (branch == current) {
                std::cout << "* " << branch << std::endl;
            } else {
                std::cout << "  " << branch << std::endl;
            }
        }
        return true;
    }

    // Validate branch name uniqueness
    if (!Validator::validateBranchNameUnique(repo, branch_name)) {
        std::cerr << "Error: " << Validator::getValidationError() << std::endl;
        return false;
    }

    // Create new branch
    if (!repo->createBranch(branch_name)) {
        std::cerr << "Error: Failed to create branch '" << branch_name << "'" << std::endl;
        return false;
    }

    std::cout << "Created branch '" << branch_name << "'" << std::endl;
    return true;
}

bool Commands::checkout(Repository* repo, const std::string& target) {
    if (!repo) {
        std::cerr << "Error: Repository pointer is null" << std::endl;
        return false;
    }

    if (target.empty()) {
        std::cerr << "Error: branch or commit required" << std::endl;
        return false;
    }

    if (!repo->open()) {
        std::cerr << "Error: Could not open repository" << std::endl;
        return false;
    }

    // 1️⃣ Try branch checkout FIRST
    if (Validator::validateBranchExists(repo, target)) {
        if (!repo->switchBranch(target)) {
            std::cerr << "Error: Failed to switch to branch '" << target << "'" << std::endl;
            return false;
        }
        std::cout << "Switched to branch '" << target << "'" << std::endl;
        return true;
    }

    // 2️⃣ If not a branch, try COMMIT checkout
    if (!Validator::validateHashFormat(target)) {
        std::cerr << "Error: Invalid commit hash format" << std::endl;
        return false;
    }

    CommitStore* store = repo->getCommitStore();
    if (!Validator::validateCommitHashExists(store, target)) {
        std::cerr << "Error: " << Validator::getValidationError() << std::endl;
        return false;
    }

    if (!repo->checkoutCommit(target)) {
        std::cerr << "Error: Failed to checkout commit '" << target << "'" << std::endl;
        return false;
    }

    std::cout << "HEAD is now at " << target << std::endl;
    return true;
}

} // namespace gitz
