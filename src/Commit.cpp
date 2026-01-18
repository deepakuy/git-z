#include "Commit.h"
#include "Hash.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>

namespace fs = std::filesystem;

namespace gitz
{
    namespace
    {
        // Private helper: serialize commit to deterministic format
        std::string serializeCommit(const Commit& commit)
        {
            std::ostringstream oss;
            oss << "tree " << commit.treeHash << "\n";
            if (!commit.parentHash.empty()) {
                oss << "parent " << commit.parentHash << "\n";
            }
            oss << "author " << commit.author << "\n";
            oss << "time " << commit.timestamp << "\n";
            oss << "message " << commit.message << "\n";
            return oss.str();
        }
    }

    CommitStore::CommitStore(const std::string& objectsDir)
        : objectsDir(objectsDir)
    {
    }

    std::string CommitStore::createCommit(
        const std::string& treeHash,
        const std::string& parentHash,
        const std::string& message)
    {
        // Validate parent commit exists (if parentHash is not empty)
        if (!parentHash.empty() && !commitExists(parentHash)) {
            std::cerr << "Error: Parent commit does not exist: " << parentHash << std::endl;
            return "";
        }

        // Create commit object
        Commit commit;
        commit.treeHash = treeHash;
        commit.parentHash = parentHash;
        commit.message = message;
        commit.author = "User";
        commit.timestamp = std::time(nullptr);

        // Serialize commit
        std::string commitContent = serializeCommit(commit);

        // Compute SHA-1 hash
        std::string hash = Hash::sha1(commitContent);

        // Construct object file path
        fs::path objectFilePath = fs::path(objectsDir) / hash;

        // Check if commit already exists (deduplication)
        if (fs::exists(objectFilePath)) {
            return hash;
        }

        // Write serialized content to object file
        try {
            std::ofstream commitFile(objectFilePath);
            if (!commitFile.is_open()) {
                std::cerr << "Error: Cannot open commit object file for writing: " << objectFilePath << std::endl;
                return "";
            }
            commitFile.write(commitContent.data(), commitContent.size());
            commitFile.close();
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return "";
        }

        return hash;
    }

    bool CommitStore::readCommit(const std::string& hash, Commit& outCommit) const
    {
        // Construct object file path
        fs::path objectFilePath = fs::path(objectsDir) / hash;

        // Check if commit object file exists
        if (!fs::exists(objectFilePath)) {
            return false;
        }

        // Read commit content
        try {
            std::ifstream commitFile(objectFilePath);
            if (!commitFile.is_open()) {
                std::cerr << "Error: Cannot open commit object file for reading: " << objectFilePath << std::endl;
                return false;
            }

            // Parse each line
            std::string line;
            while (std::getline(commitFile, line)) {
                if (line.empty()) {
                    continue;
                }

                // Split line into key and value
                size_t spacePos = line.find(' ');
                if (spacePos == std::string::npos) {
                    std::cerr << "Error: Invalid commit entry format: " << line << std::endl;
                    return false;
                }

                std::string key = line.substr(0, spacePos);
                std::string value = line.substr(spacePos + 1);

                if (key == "tree") {
                    outCommit.treeHash = value;
                } else if (key == "parent") {
                    outCommit.parentHash = value;
                } else if (key == "author") {
                    outCommit.author = value;
                } else if (key == "time") {
                    outCommit.timestamp = std::stol(value);
                } else if (key == "message") {
                    outCommit.message = value;
                }
            }
            commitFile.close();
            return true;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    bool CommitStore::commitExists(const std::string& hash) const
    {
        fs::path objectFilePath = fs::path(objectsDir) / hash;
        return fs::exists(objectFilePath);
    }

    std::vector<std::string> CommitStore::getHistory(const std::string& fromHash) const
    {
        std::vector<std::string> history;

        if (fromHash.empty()) {
            return history;
        }

        std::string currentHash = fromHash;
        while (!currentHash.empty()) {
            history.push_back(currentHash);

            // Read current commit to get parent
            Commit commit;
            if (!readCommit(currentHash, commit)) {
                // Stop if commit cannot be read
                break;
            }

            currentHash = commit.parentHash;
        }

        return history;
    }
}
