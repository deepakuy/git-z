#include <iostream>
#include <string>
#include <filesystem>
#include <iomanip>

#include "gitz.h"
#include "Index.h"   // ✅ ADD THIS

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: gitz <command> [args]" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    // Determine repository root (current working directory)
    std::string repoPath = fs::current_path().string();

    // Create repository instance
    gitz::Repository repo(repoPath);

    // Handle init command
    if (command == "init") {
        if (repo.init()) {
            std::cout << "Initialized empty Git-Z repository" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to initialize repository" << std::endl;
            return 1;
        }
    }

    // Handle add command
    if (command == "add") {
        if (argc < 3) {
            std::cout << "Usage: gitz add <filepath>" << std::endl;
            return 1;
        }
        
        // Open existing repository
        if (!repo.open()) {
            std::cerr << "Error: Repository not initialized" << std::endl;
            return 1;
        }
        
        std::string filepath = argv[2];
        if (repo.stageFile(filepath)) {
            std::cout << "Added " << filepath << " to staging" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to add file to staging" << std::endl;
            return 1;
        }
    }

    // Handle commit command
    if (command == "commit") {
        if (argc < 3) {
            std::cerr << "Usage: gitz commit \"message\"" << std::endl;
            return 1;
        }

        if (!repo.open()) {
            std::cerr << "Error: Repository not initialized" << std::endl;
            return 1;
        }

        std::string message = argv[2];

        if (repo.commit(message)) {
            return 0;
        } else {
            return 1;
        }
    }

    // Handle branch command
    if (command == "branch") {
        if (!repo.open()) {
            std::cerr << "Error: Repository not initialized" << std::endl;
            return 1;
        }

        // List branches
        if (argc == 2) {
            auto branches = repo.listBranches();
            std::string current = repo.getBranchManager() ? 
                repo.getBranchManager()->currentBranch() : "";

            for (const auto& b : branches) {
                if (b == current)
                    std::cout << "* " << b << std::endl;
                else
                    std::cout << "  " << b << std::endl;
            }
            return 0;
        }

        // Create branch
        std::string branchName = argv[2];
        if (repo.createBranch(branchName)) {
            std::cout << "Created branch '" << branchName << "'" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to create branch" << std::endl;
            return 1;
        }
    }

    // Handle checkout command
    if (command == "checkout") {
        if (argc < 3) {
            std::cerr << "Usage: gitz checkout <branch-name>" << std::endl;
            return 1;
        }

        if (!repo.open()) {
            std::cerr << "Error: Repository not initialized" << std::endl;
            return 1;
        }

        std::string branchName = argv[2];
        if (repo.switchBranch(branchName)) {
            std::cout << "Switched to branch '" << branchName << "'" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to switch to branch '" << branchName << "'" << std::endl;
            return 1;
        }
    }

    // Handle status command
    if (command == "status") {
        if (!repo.open()) {
            std::cerr << "Error: Repository not initialized" << std::endl;
            return 1;
        }

        // Get current branch
        std::string currentBranch = repo.getBranchManager() ? 
            repo.getBranchManager()->currentBranch() : "unknown";
        
        // Get HEAD commit
        std::string headCommit = repo.getHeadCommit();
        std::string shortCommit = headCommit.empty() ? "" : headCommit.substr(0, 7);

        // Print status
        std::cout << "On branch " << currentBranch << std::endl;
        if (!shortCommit.empty()) {
            std::cout << "HEAD: " << shortCommit << std::endl;
        }

        // Check staging area
        if (auto index = repo.getIndex()) {
            if (index->empty()) {
                std::cout << "Nothing to commit" << std::endl;
            } else {
                std::cout << "\nStaged changes:" << std::endl;
                for (const auto& entry : index->entries()) {
                    std::cout << "  " << entry.path << std::endl;
                }
            }
        }

        return 0;
    }

    // Handle log command
    if (command == "log") {
        std::cout << "Command 'log' not implemented yet" << std::endl;
        return 0;
    }

    // Unknown command
    std::cout << "Unknown command: " << command << std::endl;
    return 1;
}
