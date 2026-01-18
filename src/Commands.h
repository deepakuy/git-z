#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "Validator.h"

namespace gitz {

// Forward declaration
class Repository;

class Commands {
public:
    static bool init(Repository* repo, const std::string& path);
    static bool add(Repository* repo, const std::string& filepath);
    static bool commit(Repository* repo, const std::string& message);
    static void log(Repository* repo);
    static void status(Repository* repo);
    static bool branch(Repository* repo, const std::string& branch_name);
    static bool checkout(Repository* repo, const std::string& target);
    static void help();

private:
    Commands() = delete; // Prevent instantiation
};

} // namespace gitz
