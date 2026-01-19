#include "CLI.h"
#include "Commands.h"
#include "gitz.h"
#include <iostream>
#include <filesystem>

namespace gitz {

CLI::CLI(int argc, char* argv[]) 
    : argc(argc), argv(argv), command(""), help_flag(false), version_flag(false), verbose_flag(false) {
    
    if (argc >= 2) {
        // Check if argv[1] is a flag (starts with '-')
        if (argv[1][0] == '-') {
            // argv[1] is a flag, not a command
        } else {
            command = argv[1];
        }
    }
    
    // Parse remaining arguments for flags and arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            help_flag = true;
        } else if (arg == "-v" || arg == "--version") {
            version_flag = true;
        } else if (arg == "--verbose") {
            verbose_flag = true;
        } else if (arg[0] != '-' && arg != command) {
            // Non-flag argument that's not the command
            arguments.push_back(arg);
        }
    }
}

std::string CLI::getCommand() const {
    return command;
}

std::vector<std::string> CLI::getArguments() const {
    return arguments;
}

bool CLI::hasHelpFlag() const {
    return help_flag;
}

bool CLI::hasVersionFlag() const {
    return version_flag;
}

bool CLI::hasVerboseFlag() const {
    return verbose_flag;
}

bool CLI::execute() {
    if (hasHelpFlag()) {
        printUsage();
        return true;
    }
    
    if (hasVersionFlag()) {
        printVersion();
        return true;
    }
    
    if (command.empty()) {
        printUsage();
        return false;
    }
    
    // Create repository using current working directory
    Repository repo(".");
    
    if (command == "init") {
        return Commands::init(&repo, ".");
    }
    
    if (!repo.open()) {
        std::cerr << "Error: Failed to open repository" << std::endl;
        return false;
    }
    
    if (command == "add") {
        if (arguments.empty()) {
            std::cerr << "Error: add requires a file path" << std::endl;
            return false;
        }
        return Commands::add(&repo, arguments[0]);
    }
    
    if (command == "commit") {
        std::string message;
        
        // Handle both formats: "-m message" and direct message
        if (arguments.empty()) {
            std::cerr << "Error: commit message cannot be empty" << std::endl;
            return false;
        }
        
        if (arguments[0] == "-m") {
            // Format: gitz commit -m "message"
            if (arguments.size() < 2) {
                std::cerr << "Error: commit message cannot be empty" << std::endl;
                return false;
            }
            message = arguments[1];
        } else {
            // Format: gitz commit "message"
            message = arguments[0];
        }
        
        // Reject empty messages
        if (message.empty()) {
            std::cerr << "Error: commit message cannot be empty" << std::endl;
            return false;
        }
        
        return Commands::commit(&repo, message);
    }
    
    if (command == "log") {
        Commands::log(&repo);
        return true;
    }
    
    if (command == "status") {
        Commands::status(&repo);
        return true;
    }
    
    if (command == "branch") {
        if (arguments.empty()) {
            return Commands::branch(&repo, "");
        } else {
            return Commands::branch(&repo, arguments[0]);
        }
    }
    
    if (command == "checkout") {
        if (arguments.empty()) {
            std::cerr << "Error: checkout requires a branch or commit hash" << std::endl;
            return false;
        }
        return Commands::checkout(&repo, arguments[0]);
    }
    
    if (command == "optimize") {
        Optimizer::analyzeRepositorySize(repo.getRepoPath());
        return true;
    }
    
    std::cerr << "Unknown command: " << command << std::endl;
    printUsage();
    return false;
}

void CLI::printWelcome() {
    std::cout << "Git-Z - Simple Version Control System" << std::endl;
    std::cout << "Type 'gitz --help' for usage information" << std::endl;
}

void CLI::printUsage() {
    std::cout << "Usage: gitz <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  init        Initialize a new repository" << std::endl;
    std::cout << "  add <file>  Add file to staging area" << std::endl;
    std::cout << "  commit -m    Commit staged changes" << std::endl;
    std::cout << "  log         Show commit history" << std::endl;
    std::cout << "  status       Show working directory status" << std::endl;
    std::cout << "  branch       List or create branches" << std::endl;
    std::cout << "  checkout     Switch branches or restore commits" << std::endl;
    std::cout << "  optimize     Analyze repository size" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help    Show this help message" << std::endl;
    std::cout << "  -v, --version  Show version information" << std::endl;
    std::cout << "  --verbose      Enable verbose output" << std::endl;
}

void CLI::printVersion() {
    std::cout << "Git-Z v1.0.0" << std::endl;
}

void CLI::printCommandHelp(const std::string& command) {
    std::cout << "Help for command: " << command << std::endl;
    std::cout << "Use 'gitz --help' for general usage information" << std::endl;
}

} // namespace gitz
