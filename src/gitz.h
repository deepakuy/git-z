#pragma once

#include <string>

namespace gitz
{
    class Repository
    {
    private:
        std::string repoPath;

    public:
        explicit Repository(const std::string& path);
        bool init();
        std::string getRepoPath() const;
    };
}
