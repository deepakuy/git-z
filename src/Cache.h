#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <vector>

namespace gitz {

class BlobCache {
private:
    std::unordered_map<std::string, std::string> cache;
    std::list<std::string> access_order;
    size_t max_size;

public:
    explicit BlobCache(size_t max_size);
    void put(const std::string& hash, const std::string& content);
    bool get(const std::string& hash, std::string& out_content);
    void clear();
    size_t size() const;
};

class CommitCache {
private:
    std::unordered_map<std::string, std::string> cache;
    std::list<std::string> access_order;
    size_t max_size;

public:
    explicit CommitCache(size_t max_size);
    void put(const std::string& hash, const std::string& serialized_commit);
    bool get(const std::string& hash, std::string& out_commit);
    void clear();
    size_t size() const;
};

} // namespace gitz
