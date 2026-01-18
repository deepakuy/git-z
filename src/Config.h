#pragma once


#include <string>
#include <cstddef>

namespace gitz {

// Compile-time constants
inline constexpr const char* VERSION = "1.0.0";
inline constexpr const char* GITZ_DIR = ".gitz";
inline constexpr const char* OBJECTS_DIR = ".gitz/objects";
inline constexpr const char* REFS_DIR = ".gitz/refs";
inline constexpr const char* HEADS_DIR = ".gitz/refs/heads";
inline constexpr const char* HEAD_FILE = ".gitz/HEAD";
inline constexpr const char* INDEX_FILE = ".gitz/index";
inline constexpr const char* DEFAULT_BRANCH = "master";
inline constexpr const char* DEFAULT_AUTHOR = "User";

// Cache size constants
inline constexpr size_t BLOB_CACHE_SIZE = 100;
inline constexpr size_t COMMIT_CACHE_SIZE = 50;

// Hash constants
inline constexpr size_t HASH_LENGTH = 40; // SHA-1 hex length

class Config {
public:
    static std::string version() { return VERSION; }
    static std::string gitzDir() { return GITZ_DIR; }
    static std::string objectsDir() { return OBJECTS_DIR; }
    static std::string refsDir() { return REFS_DIR; }
    static std::string headsDir() { return HEADS_DIR; }
    static std::string headFile() { return HEAD_FILE; }
    static std::string indexFile() { return INDEX_FILE; }
    static std::string defaultBranch() { return DEFAULT_BRANCH; }
    static std::string defaultAuthor() { return DEFAULT_AUTHOR; }
    static size_t blobCacheSize() { return BLOB_CACHE_SIZE; }
    static size_t commitCacheSize() { return COMMIT_CACHE_SIZE; }
};

} // namespace gitz
