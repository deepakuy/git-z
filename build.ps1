# Git-Z Build Script
# Automated build process for Git-Z version control system

Write-Host "Building Git-Z..."

# Clean previous build
if (Test-Path "build") {
    Write-Host "Removing previous build directory..."
    Remove-Item -Recurse -Force "build"
}

# Create build directory
Write-Host "Creating build directory..."
New-Item -ItemType Directory -Force "build"

# Change to build directory
Set-Location "build"

# Configure with CMake
Write-Host "Configuring project..."
$cmakeResult = cmake ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!"
    Write-Host "Error: $cmakeResult"
    exit 1
}

# Build the project
Write-Host "Building project..."
$buildResult = cmake --build .
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!"
    Write-Host "Error: $buildResult"
    exit 1
}

# Build complete
Write-Host "Build complete!"
Write-Host "Binary location: ./bin/Debug/gitz.exe"

# Return to project root
Set-Location ".."

Write-Host "Git-Z is ready to use!"
