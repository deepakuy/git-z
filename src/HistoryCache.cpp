#include "HistoryCache.h"

namespace gitz {

HistoryCache::HistoryCache() {}

void HistoryCache::put(const std::string& start_hash, const std::vector<std::string>& history) {
    cache[start_hash] = history;
}

bool HistoryCache::get(const std::string& start_hash, std::vector<std::string>& out_history) {
    auto it = cache.find(start_hash);
    if (it == cache.end()) {
        return false;
    }
    
    out_history = it->second;
    return true;
}

void HistoryCache::invalidate(const std::string& hash) {
    // Iterate through cache and remove entries containing the hash
    auto it = cache.begin();
    while (it != cache.end()) {
        bool contains_hash = false;
        for (const std::string& cached_hash : it->second) {
            if (cached_hash == hash) {
                contains_hash = true;
                break;
            }
        }
        
        if (contains_hash) {
            it = cache.erase(it);
        } else {
            ++it;
        }
    }
}

void HistoryCache::clear() {
    cache.clear();
}

size_t HistoryCache::size() const {
    return cache.size();
}

} // namespace gitz
