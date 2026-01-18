#include "Blob.h"
#include "Hash.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace gitz
{
    BlobStore::BlobStore(const std::string& objectsDir)
        : objectsDir(objectsDir)
    {
    }

    std::string BlobStore::addBlobFromFile(const std::string& filePath)
    {
        // Read file content in binary mode
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for reading: " << filePath << std::endl;
            return "";
        }

        // Read entire file content
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        // Compute SHA-1 hash of content
        std::string hash = Hash::sha1(content);

        // Construct object file path
        fs::path objectFilePath = fs::path(objectsDir) / hash;

        // Check if blob already exists (deduplication)
        if (fs::exists(objectFilePath)) {
            // Blob already exists, return hash without overwriting
            return hash;
        }

        // Write content to object file
        try {
            std::ofstream objectFile(objectFilePath, std::ios::binary);
            if (!objectFile.is_open()) {
                std::cerr << "Error: Cannot open object file for writing: " << objectFilePath << std::endl;
                return "";
            }
            objectFile.write(content.data(), content.size());
            objectFile.close();
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return "";
        }

        return hash;
    }

    bool BlobStore::readBlob(const std::string& hash, std::string& outContent) const
    {
        // Construct object file path
        fs::path objectFilePath = fs::path(objectsDir) / hash;

        // Check if object file exists
        if (!fs::exists(objectFilePath)) {
            return false;
        }

        // Read blob content
        try {
            std::ifstream objectFile(objectFilePath, std::ios::binary);
            if (!objectFile.is_open()) {
                std::cerr << "Error: Cannot open object file for reading: " << objectFilePath << std::endl;
                return false;
            }

            outContent = std::string((std::istreambuf_iterator<char>(objectFile)),
                                     std::istreambuf_iterator<char>());
            objectFile.close();
            return true;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    bool BlobStore::blobExists(const std::string& hash) const
    {
        fs::path objectFilePath = fs::path(objectsDir) / hash;
        return fs::exists(objectFilePath);
    }
}
