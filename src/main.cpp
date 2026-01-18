#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: gitz <command> [args]" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    // List of supported commands
    if (command == "init" ||
        command == "add" ||
        command == "commit" ||
        command == "log" ||
        command == "status" ||
        command == "branch" ||
        command == "checkout") {
        std::cout << "Command '" << command << "' not implemented yet" << std::endl;
        return 0;
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        return 1;
    }
}
