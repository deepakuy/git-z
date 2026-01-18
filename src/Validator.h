#pragma once

#include <string>
#include <vector>

namespace gitz {

// Forward declarations
class Repository;
class Index;
class BlobStore;
class TreeStore;
class CommitStore;
struct TreeEntry;

class Validator {
private:
    static std::string lastError;

public:
    // Repository validation
    static bool validateRepositoryStructure(const std::string& repo_path);
    static bool validateNoNestedRepo(const std::string& repo_path);

    // File and staging validation
    static bool validateFileExists(const std::string& filepath);
    static bool validateFilenameUniqueness(const std::vector<TreeEntry>& entries);
    static bool validateStagingAreaNotEmpty(Index* index);
    static bool validateCommitMessage(const std::string& message);
    static bool validateParentCommitExists(CommitStore* store, const std::string& parent_hash);

    // Branch validation
    static bool validateBranchNameUnique(Repository* repo, const std::string& branch_name);
    static bool validateBranchExists(Repository* repo, const std::string& branch_name);

    // Hash existence validation
    static bool validateCommitHashExists(CommitStore* store, const std::string& hash);
    static bool validateBlobHashExists(BlobStore* store, const std::string& hash);
    static bool validateTreeHashExists(TreeStore* store, const std::string& hash);

    // Format validation
    static bool validateHashFormat(const std::string& hash);

    // Corruption validation
    static bool validateCorruptedBlob(BlobStore* store, const std::string& hash);
    static bool validateCorruptedTree(TreeStore* store, const std::string& hash);
    static bool validateCorruptedCommit(CommitStore* store, const std::string& hash);

    // Error handling
    static std::string getValidationError();
};

} // namespace gitz
