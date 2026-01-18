#include "Validator.h"
#include "Repository.h"
#include "Index.h"
#include "Blob.h"
#include "Tree.h"
#include "Commit.h"
#include "Hash.h"
#include <filesystem>
#include <fstream>
#include <unordered_set>

namespace fs = std::filesystem;

namespace gitz
{
    std::string Validator::lastError = "";

    bool Validator::validateRepositoryStructure(const std::string& repo_path)
    {
        lastError.clear();

        const fs::path base(repo_path);
        const fs::path gitzDir = base / ".gitz";

        const fs::path objectsDir = gitzDir / "objects";
        const fs::path refsDir = gitzDir / "refs";
        const fs::path headsDir = refsDir / "heads";
        const fs::path headFile = gitzDir / "HEAD";
        const fs::path indexFile = gitzDir / "index";

        if (!fs::exists(gitzDir) || !fs::exists(objectsDir) || !fs::exists(refsDir) ||
            !fs::exists(headsDir) || !fs::exists(headFile) || !fs::exists(indexFile)) {
            lastError = "Invalid repository structure";
            return false;
        }

        return true;
    }

    bool Validator::validateNoNestedRepo(const std::string& repo_path)
    {
        lastError.clear();

        std::filesystem::path current = std::filesystem::absolute(repo_path);

        while (true) {
            if (std::filesystem::exists(current / ".gitz")) {
                lastError = "Nested Git-Z repository not allowed";
                return false;
            }

            if (current.has_parent_path()) {
                auto parent = current.parent_path();
                if (parent == current) break;
                current = parent;
            } else {
                break;
            }
        }

        return true;
    }


    bool Validator::validateFileExists(const std::string& filepath)
    {
        lastError.clear();

        if (!fs::exists(fs::path(filepath))) {
            lastError = "File does not exist";
            return false;
        }

        return true;
    }

    bool Validator::validateFilenameUniqueness(const std::vector<TreeEntry>& entries)
    {
        lastError.clear();

        std::unordered_set<std::string> seen;
        for (const auto& entry : entries) {
            if (seen.find(entry.name) != seen.end()) {
                lastError = "Duplicate filename in tree";
                return false;
            }
            seen.insert(entry.name);
        }

        return true;
    }

    bool Validator::validateStagingAreaNotEmpty(Index* index)
    {
        lastError.clear();

        if (index == nullptr || index->empty()) {
            lastError = "nothing to commit, working tree clean";
            return false;
        }

        return true;
    }

    bool Validator::validateCommitMessage(const std::string& message)
    {
        lastError.clear();

        if (message.empty() || message.size() > 256) {
            lastError = "Invalid commit message";
            return false;
        }

        return true;
    }

    bool Validator::validateParentCommitExists(CommitStore* store, const std::string& parent_hash)
    {
        lastError.clear();

        if (parent_hash.empty()) {
            return true;
        }

        if (store == nullptr || !store->commitExists(parent_hash)) {
            lastError = "Parent commit does not exist";
            return false;
        }

        return true;
    }

    bool Validator::validateBranchNameUnique(Repository* repo, const std::string& branch_name)
    {
        lastError.clear();

        if (repo == nullptr) {
            lastError = "Branch already exists";
            return false;
        }

        const auto branches = repo->listBranches();
        for (const auto& b : branches) {
            if (b == branch_name) {
                lastError = "Branch already exists";
                return false;
            }
        }

        return true;
    }

    bool Validator::validateBranchExists(Repository* repo, const std::string& branch_name)
    {
        lastError.clear();

        if (repo == nullptr) {
            lastError = "Branch does not exist";
            return false;
        }

        const auto branches = repo->listBranches();
        for (const auto& b : branches) {
            if (b == branch_name) {
                return true;
            }
        }

        lastError = "Branch does not exist";
        return false;
    }

    bool Validator::validateCommitHashExists(CommitStore* store, const std::string& hash)
    {
        lastError.clear();

        if (store == nullptr || !store->commitExists(hash)) {
            lastError = "Commit hash not found";
            return false;
        }

        return true;
    }

    bool Validator::validateBlobHashExists(BlobStore* store, const std::string& hash)
    {
        lastError.clear();

        if (store == nullptr || !store->blobExists(hash)) {
            lastError = "Blob hash not found";
            return false;
        }

        return true;
    }

    bool Validator::validateTreeHashExists(TreeStore* store, const std::string& hash)
    {
        lastError.clear();

        if (store == nullptr || !store->treeExists(hash)) {
            lastError = "Tree hash not found";
            return false;
        }

        return true;
    }

    bool Validator::validateHashFormat(const std::string& hash)
    {
        lastError.clear();

        if (hash.size() != 40) {
            lastError = "Invalid hash format";
            return false;
        }

        for (char c : hash) {
            const bool isDigit = (c >= '0' && c <= '9');
            const bool isLowerHex = (c >= 'a' && c <= 'f');
            const bool isUpperHex = (c >= 'A' && c <= 'F');
            if (!isDigit && !isLowerHex && !isUpperHex) {
                lastError = "Invalid hash format";
                return false;
            }
        }

        return true;
    }

    bool Validator::validateCorruptedBlob(BlobStore* store, const std::string& hash)
    {
        lastError.clear();

        if (store == nullptr) {
            lastError = "Corrupted blob detected";
            return false;
        }

        std::string content;
        if (!store->readBlob(hash, content)) {
            lastError = "Corrupted blob detected";
            return false;
        }

        const std::string computed = Hash::sha1(content);
        if (computed != hash) {
            lastError = "Corrupted blob detected";
            return false;
        }

        return true;
    }

    bool Validator::validateCorruptedTree(TreeStore* store, const std::string& hash)
    {
        lastError.clear();

        if (store == nullptr) {
            lastError = "Corrupted tree detected";
            return false;
        }

        Tree tree;
        if (!store->readTree(hash, tree)) {
            lastError = "Corrupted tree detected";
            return false;
        }

        // Serialize tree to deterministic text format, matching TreeStore::createTree()
        std::string serialized;
        for (const auto& entry : tree.entries) {
            serialized += (entry.isDirectory ? "tree" : "blob");
            serialized += " ";
            serialized += entry.name;
            serialized += " ";
            serialized += entry.hash;
            serialized += "\n";
        }

        const std::string computed = Hash::sha1(serialized);
        if (computed != hash) {
            lastError = "Corrupted tree detected";
            return false;
        }

        return true;
    }

    bool Validator::validateCorruptedCommit(CommitStore* store, const std::string& hash)
    {
        lastError.clear();

        if (store == nullptr) {
            lastError = "Corrupted commit detected";
            return false;
        }

        Commit commit;
        if (!store->readCommit(hash, commit)) {
            lastError = "Corrupted commit detected";
            return false;
        }

        // Serialize commit to deterministic format, matching CommitStore::createCommit()
        std::string serialized;
        serialized += "tree ";
        serialized += commit.treeHash;
        serialized += "\n";
        if (!commit.parentHash.empty()) {
            serialized += "parent ";
            serialized += commit.parentHash;
            serialized += "\n";
        }
        serialized += "author ";
        serialized += commit.author;
        serialized += "\n";
        serialized += "time ";
        serialized += std::to_string(commit.timestamp);
        serialized += "\n";
        serialized += "message ";
        serialized += commit.message;
        serialized += "\n";

        const std::string computed = Hash::sha1(serialized);
        if (computed != hash) {
            lastError = "Corrupted commit detected";
            return false;
        }

        return true;
    }

    std::string Validator::getValidationError()
    {
        return lastError;
    }
}
