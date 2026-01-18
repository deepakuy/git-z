#pragma once

#include <string>
#include <vector>

namespace gitz
{
    struct TreeEntry
    {
        std::string name;        // File or directory name
        std::string hash;        // Blob hash or subtree hash
        bool isDirectory;        // True for subtree, false for blob
    };

    struct Tree
    {
        std::vector<TreeEntry> entries;
    };

    class TreeStore
    {
    private:
        std::string objectsDir;

    public:
        explicit TreeStore(const std::string& objectsDir);

        std::string createTree(const std::vector<TreeEntry>& entries);
        bool readTree(const std::string& hash, Tree& outTree) const;
        bool treeExists(const std::string& hash) const;
    };
}
