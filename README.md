# Git-Z: Distributed Version Control System

A lightweight C++17 implementation of Git for educational purposes.

## Overview

Git-Z is a simplified version control system inspired by Git, built completely from scratch in modern C++17. It demonstrates core version control concepts including content-addressable storage, commit history, branching, and repository management.

## Core Features

- **Repository initialization** - Create new version control repositories
- **File staging** - Add files to staging area before committing
- **Commit creation** - Create snapshots with SHA-1 hashes and history
- **Branch management** - Create, list, and switch between branches
- **Detached HEAD checkout** - Switch to specific commits directly
- **Repository status** - Show working directory and staging state
- **Commit history** - Browse through commit log with full details
- **Content deduplication** - Store identical content only once using SHA-1
- **Repository analysis** - Analyze storage usage and object statistics
- **Caching system** - LRU caches for blobs, commits, and history
- **Command-line interface** - Clean CLI with help and error handling

## Architecture

### Data Storage

- **Blobs**: Store raw file content, named by SHA-1 hash
- **Trees**: Map file paths to blob hashes (directory structure)
- **Commits**: Snapshots with parent pointers, timestamps, and messages
- **HEAD**: Points to current branch or commit (detached state)
- **Index**: Staging area for files to be committed

### Key Components

- **Repository**: Main class managing all operations
- **Commands**: Static methods for command routing and validation
- **Validator**: Repository validation and error checking
- **Cache**: LRU caching system for performance optimization
- **Optimizer**: Repository analysis and storage statistics
- **CLI**: Command-line argument parsing and execution

## Build Instructions

### Requirements
- C++17 compatible compiler
- CMake 3.10 or higher
- Windows (Visual Studio 2019+) or Linux/macOS

### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Binary generated in `bin/gitz.exe`

### Linux/macOS
```bash
mkdir build
cd build
cmake ..
make
```

## Usage Examples

```bash
# Initialize a new repository
gitz init

# Add files to staging
gitz add README.md
gitz add src/main.cpp

# Commit changes
gitz commit -m "Initial implementation"

# View commit history
gitz log

# Show repository status
gitz status

# Branch operations
gitz branch                    # List branches
gitz branch feature             # Create new branch
gitz checkout feature           # Switch to branch

# Checkout specific commit
gitz checkout a1b2c3d4e5f6

# Analyze repository
gitz optimize
```

## Project Structure

```
git-z/
├── src/                    # Source files
│   ├── main.cpp           # Application entry point
│   ├── CLI.h/.cpp         # Command-line interface
│   ├── Commands.h/.cpp     # Command routing
│   ├── Repository.h/.cpp   # Main repository class
│   ├── Validator.h/.cpp    # Validation logic
│   ├── Cache.h/.cpp        # LRU caching system
│   ├── HistoryCache.h/.cpp # History traversal cache
│   ├── Optimizer.h/.cpp   # Repository analysis
│   └── [Store classes]   # Blob, Tree, Commit, Index, Branch
├── CMakeLists.txt           # Build configuration
├── README.md               # This file
└── bin/                   # Compiled binaries
```

## Design Principles

- **Educational focus**: Clear, readable code for learning
- **No external dependencies**: Built entirely from scratch
- **Modern C++**: Uses C++17 features and best practices
- **Content-addressable**: All objects stored by SHA-1 hash
- **Immutable objects**: Once written, objects never change
- **Simple operations**: Each command does one thing well
- **Clean architecture**: Separation of concerns between components

## Implementation Notes

### Storage Format
- Objects stored in `.gitz/objects/` directory
- Each object is a file named by its SHA-1 hash
- No compression or packing (educational simplicity)
- Human-readable file formats for inspection

### Security Considerations
- SHA-1 for content integrity (educational use)
- No cryptographic security guarantees
- Local repository access only
- No networking or remote operations

### Performance Features
- LRU caching for frequently accessed objects
- Lazy loading of repository data
- Efficient directory traversal
- Minimal memory footprint

## Limitations

- Single-threaded operations only
- No remote repository support
- No merge operations implemented
- No conflict resolution
- SHA-1 only (no SHA-256 support)
- Educational/academic use case focus

## Contributing

This project is designed for educational purposes to demonstrate version control concepts. Contributions should focus on:

1. Code clarity and educational value
2. Additional Git-like features
3. Performance improvements
4. Cross-platform compatibility
5. Documentation enhancements

## License

Educational / Academic use. See LICENSE file for details.

---

**Git-Z** - Learn version control by building it from scratch in modern C++.
