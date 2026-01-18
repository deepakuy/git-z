#pragma once

#include <string>
#include <vector>
#include <cstddef>

namespace gitz {

// Forward declarations MUST be inside namespace gitz
class BlobStore;
class TreeStore;
class CommitStore;
class BlobCache;
class CommitCache;

class Optimizer {
public:
    static void analyzeRepositorySize(const std::string& repo_path);

    static size_t estimateStorageUsed(
        BlobStore* blob_store,
        TreeStore* tree_store,
        CommitStore* commit_store
    );

    static bool verifyDeduplication(BlobStore* blob_store);

    static bool verifyTreeIntegrity(TreeStore* tree_store);

    static void optimizeMemoryUsage(
        BlobCache* blob_cache,
        CommitCache* commit_cache,
        size_t repo_size_bytes
    );

private:
    Optimizer() = delete;
};

} // namespace gitz
