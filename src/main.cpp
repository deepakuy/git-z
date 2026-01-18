#include <iostream>
#include <string>
#include <filesystem>
#include <iomanip>

#include "gitz.h"
#include "Commands.h"

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
        return gitz::Commands::init(&repo, repoPath) ? 0 : 1;
    }

    // Handle add command
    if (command == "add") {
        if (argc < 3) {
            std::cerr << "Usage: gitz add <file>" << std::endl;
            return 1;
        }
        return gitz::Commands::add(&repo, argv[2]) ? 0 : 1;
    }

    // Handle commit command
    if (command == "commit") {
        if (argc < 3) {
            std::cerr << "Usage: gitz commit \"message\"" << std::endl;
            return 1;
        }
        return gitz::Commands::commit(&repo, argv[2]) ? 0 : 1;
    }

    // Handle branch command
    if (command == "branch") {
        std::string name = (argc >= 3) ? argv[2] : "";
        return gitz::Commands::branch(&repo, name) ? 0 : 1;
    }

    // Handle checkout command
    if (command == "checkout") {
        if (argc < 3) {
            std::cerr << "Usage: gitz checkout <branch|commit>" << std::endl;
            return 1;
        }
        return gitz::Commands::checkout(&repo, argv[2]) ? 0 : 1;
    }

    // Handle status command
    if (command == "status") {
        gitz::Commands::status(&repo);
        return 0;
    }

    // Handle log command
    if (command == "log") {
        if (!repo.open()) {
            std::cerr << "Error: Repository not initialized" << std::endl;
            return 1;
        }
        repo.log();
        return 0;
    }


    // Unknown command
    std::cout << "Unknown command: " << command << std::endl;
    return 1;
}
