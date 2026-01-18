#include <iostream>
#include <string>
#include <filesystem>
#include "gitz.h"

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

    // Placeholder commands
    if (command == "log" ||
        command == "status" ||
        command == "branch" ||
        command == "checkout") {
        std::cout << "Command '" << command << "' not implemented yet" << std::endl;
        return 0;
    }

    // Unknown command
    std::cout << "Unknown command: " << command << std::endl;
    return 1;
}
