#include "Tree.h"
#include "Hash.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <sstream>

namespace fs = std::filesystem;

namespace gitz
{
    TreeStore::TreeStore(const std::string& objectsDir)
        : objectsDir(objectsDir)
    {
    }

    std::string TreeStore::createTree(const std::vector<TreeEntry>& entries)
    {
        // Validate unique entry names
        std::unordered_set<std::string> nameSet;
        for (const auto& entry : entries) {
            if (nameSet.find(entry.name) != nameSet.end()) {
                std::cerr << "Error: Duplicate entry name in tree: " << entry.name << std::endl;
                return "";
            }
            nameSet.insert(entry.name);
        }

        // Serialize entries into deterministic text format
        std::ostringstream serialized;
        for (const auto& entry : entries) {
            std::string type = entry.isDirectory ? "tree" : "blob";
            serialized << type << " " << entry.name << " " << entry.hash << "\n";
        }
        std::string treeContent = serialized.str();

        // Compute SHA-1 hash of serialized content
        std::string hash = Hash::sha1(treeContent);

        // Construct object file path
        fs::path objectFilePath = fs::path(objectsDir) / hash;

        // Check if tree object already exists (deduplication)
        if (fs::exists(objectFilePath)) {
            return hash;
        }

        // Write serialized content to object file
        try {
            std::ofstream treeFile(objectFilePath);
            if (!treeFile.is_open()) {
                std::cerr << "Error: Cannot open tree object file for writing: " << objectFilePath << std::endl;
                return "";
            }
            treeFile.write(treeContent.data(), treeContent.size());
            treeFile.close();
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return "";
        }

        return hash;
    }

    bool TreeStore::readTree(const std::string& hash, Tree& outTree) const
    {
        // Construct object file path
        fs::path objectFilePath = fs::path(objectsDir) / hash;

        // Check if tree object file exists
        if (!fs::exists(objectFilePath)) {
            return false;
        }

        // Read tree content
        try {
            std::ifstream treeFile(objectFilePath);
            if (!treeFile.is_open()) {
                std::cerr << "Error: Cannot open tree object file for reading: " << objectFilePath << std::endl;
                return false;
            }

            // Parse each line to reconstruct TreeEntry objects
            std::string line;
            outTree.entries.clear();
            while (std::getline(treeFile, line)) {
                if (line.empty()) {
                    continue;
                }

                std::istringstream iss(line);
                std::string type, name, entryHash;
                if (!(iss >> type >> name >> entryHash)) {
                    std::cerr << "Error: Invalid tree entry format: " << line << std::endl;
                    return false;
                }

                TreeEntry entry;
                entry.name = name;
                entry.hash = entryHash;
                entry.isDirectory = (type == "tree");
                outTree.entries.push_back(entry);
            }
            treeFile.close();
            return true;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    bool TreeStore::treeExists(const std::string& hash) const
    {
        fs::path objectFilePath = fs::path(objectsDir) / hash;
        return fs::exists(objectFilePath);
    }
}
