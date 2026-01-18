cmake_minimum_required(VERSION 3.10)
project(Git-Z)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings (portable across Linux and Windows)
if(MSVC)
    # MSVC compiler
    add_compile_options(/W4)
else()
    # GCC and Clang
    add_compile_options(-Wall -Wextra)
endif()

# Set runtime output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Create executable target
add_executable(gitz
    src/main.cpp
    src/Repository.cpp
    src/Hash.cpp
    src/Blob.cpp
    src/Tree.cpp
)

# Add include directory
target_include_directories(gitz PRIVATE src)
