#pragma once

#include <string>
#include <vector>

namespace gitz
{
    struct Commit
    {
        std::string treeHash;      // Tree hash for this commit
        std::string parentHash;    // Parent commit hash (empty if root)
        std::string message;       // Commit message
        std::string author;        // Author name
        long timestamp;            // Unix timestamp
    };

    class CommitStore
    {
    private:
        std::string objectsDir;

    public:
        explicit CommitStore(const std::string& objectsDir);

        std::string createCommit(
            const std::string& treeHash,
            const std::string& parentHash,
            const std::string& message
        );

        bool readCommit(const std::string& hash, Commit& outCommit) const;
        bool commitExists(const std::string& hash) const;
        std::vector<std::string> getHistory(const std::string& fromHash) const;
    };
}
