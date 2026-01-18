#include "Cache.h"

namespace gitz {

// BlobCache Implementation
BlobCache::BlobCache(size_t max_size) : max_size(max_size) {}

void BlobCache::put(const std::string& hash, const std::string& content) {
    // If hash already exists, remove it from access order
    auto it = cache.find(hash);
    if (it != cache.end()) {
        access_order.remove(hash);
    }
    
    // Insert/update content in cache
    cache[hash] = content;
    
    // Push hash to back of access order (most recently used)
    access_order.push_back(hash);
    
    // If cache size exceeds max_size, remove least recently used
    while (cache.size() > max_size) {
        const std::string& lru_hash = access_order.front();
        cache.erase(lru_hash);
        access_order.pop_front();
    }
}

bool BlobCache::get(const std::string& hash, std::string& out_content) {
    auto it = cache.find(hash);
    if (it == cache.end()) {
        return false;
    }
    
    // Set output content
    out_content = it->second;
    
    // Move hash to back of access order (most recently used)
    access_order.remove(hash);
    access_order.push_back(hash);
    
    return true;
}

void BlobCache::clear() {
    cache.clear();
    access_order.clear();
}

size_t BlobCache::size() const {
    return cache.size();
}

// CommitCache Implementation
CommitCache::CommitCache(size_t max_size) : max_size(max_size) {}

void CommitCache::put(const std::string& hash, const std::string& serialized_commit) {
    // If hash already exists, remove it from access order
    auto it = cache.find(hash);
    if (it != cache.end()) {
        access_order.remove(hash);
    }
    
    // Insert/update serialized commit in cache
    cache[hash] = serialized_commit;
    
    // Push hash to back of access order (most recently used)
    access_order.push_back(hash);
    
    // If cache size exceeds max_size, remove least recently used
    while (cache.size() > max_size) {
        const std::string& lru_hash = access_order.front();
        cache.erase(lru_hash);
        access_order.pop_front();
    }
}

bool CommitCache::get(const std::string& hash, std::string& out_commit) {
    auto it = cache.find(hash);
    if (it == cache.end()) {
        return false;
    }
    
    // Set output commit
    out_commit = it->second;
    
    // Move hash to back of access order (most recently used)
    access_order.remove(hash);
    access_order.push_back(hash);
    
    return true;
}

void CommitCache::clear() {
    cache.clear();
    access_order.clear();
}

size_t CommitCache::size() const {
    return cache.size();
}

} // namespace gitz
