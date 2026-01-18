#include "Index.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <ctime>

namespace gitz
{
    namespace
    {
        // Private helper: save index to file
        void save(const std::string& indexFilePath, std::vector<IndexEntry> entries)
        {
            // Sort entries alphabetically by path
            std::sort(entries.begin(), entries.end(),
                [](const IndexEntry& a, const IndexEntry& b) {
                    return a.path < b.path;
                });

            // Write to file
            std::ofstream indexFile(indexFilePath);
            if (!indexFile.is_open()) {
                return;
            }

            for (const auto& entry : entries) {
                indexFile << entry.path << "\t" << entry.blobHash << "\t" << entry.timestamp << "\n";
            }
            indexFile.close();
        }

        // Private helper: load index from file
        void load(const std::string& indexFilePath, std::vector<IndexEntry>& entries)
        {
            std::ifstream indexFile(indexFilePath);
            if (!indexFile.is_open()) {
                return;
            }

            entries.clear();
            std::string line;
            while (std::getline(indexFile, line)) {
                if (line.empty()) {
                    continue;
                }

                // Parse tab-separated values
                std::istringstream iss(line);
                std::string path, blobHash, timestampStr;

                if (!std::getline(iss, path, '\t') ||
                    !std::getline(iss, blobHash, '\t') ||
                    !std::getline(iss, timestampStr)) {
                    continue;
                }

                IndexEntry entry;
                entry.path = path;
                entry.blobHash = blobHash;
                try {
                    entry.timestamp = std::stol(timestampStr);
                } catch (...) {
                    continue;
                }
                entries.push_back(entry);
            }
            indexFile.close();
        }
    }

    Index::Index(const std::string& indexFilePath)
        : indexFilePath(indexFilePath)
    {
        load(indexFilePath, indexEntries);
    }

    bool Index::add(const std::string& path, const std::string& blobHash)
    {
        // Check if entry already exists
        auto it = std::find_if(indexEntries.begin(), indexEntries.end(),
            [&path](const IndexEntry& entry) { return entry.path == path; });

        if (it != indexEntries.end()) {
            // Update existing entry
            it->blobHash = blobHash;
            it->timestamp = std::time(nullptr);
        } else {
            // Add new entry
            IndexEntry entry;
            entry.path = path;
            entry.blobHash = blobHash;
            entry.timestamp = std::time(nullptr);
            indexEntries.push_back(entry);
        }

        // Persist changes
        save(indexFilePath, indexEntries);
        return true;
    }

    bool Index::remove(const std::string& path)
    {
        auto it = std::find_if(indexEntries.begin(), indexEntries.end(),
            [&path](const IndexEntry& entry) { return entry.path == path; });

        if (it != indexEntries.end()) {
            indexEntries.erase(it);
            save(indexFilePath, indexEntries);
            return true;
        }

        return false;
    }

    bool Index::contains(const std::string& path) const
    {
        return std::find_if(indexEntries.begin(), indexEntries.end(),
            [&path](const IndexEntry& entry) { return entry.path == path; })
            != indexEntries.end();
    }

    std::vector<IndexEntry> Index::entries() const
    {
        return indexEntries;
    }

    bool Index::empty() const
    {
        return indexEntries.empty();
    }

    void Index::clear()
    {
        indexEntries.clear();
        save(indexFilePath, indexEntries);
    }
}
