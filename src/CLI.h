#pragma once

#include <string>
#include <vector>

namespace gitz {

class CLI {
private:
    int argc;
    char** argv;
    std::string command;
    std::vector<std::string> arguments;
    bool help_flag;
    bool version_flag;
    bool verbose_flag;

public:
    CLI(int argc, char* argv[]);
    
    std::string getCommand() const;
    std::vector<std::string> getArguments() const;
    bool hasHelpFlag() const;
    bool hasVersionFlag() const;
    bool hasVerboseFlag() const;
    bool execute();

private:
    void printWelcome();
    void printUsage();
    void printVersion();
    void printCommandHelp(const std::string& command);
};

} // namespace gitz
