#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace gitz {

class HistoryCache {
private:
    std::unordered_map<std::string, std::vector<std::string>> cache;

public:
    HistoryCache();
    void put(const std::string& start_hash, const std::vector<std::string>& history);
    bool get(const std::string& start_hash, std::vector<std::string>& out_history);
    void invalidate(const std::string& hash);
    void clear();
    size_t size() const;
};

} // namespace gitz
