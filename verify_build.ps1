# Git-Z Build Verification Script
# Verifies that Git-Z builds correctly and all components work

# Capture project root directory at start
$projectRoot = Get-Location

Write-Host "Verifying Git-Z build..."

# Resolve binary path at the top (before any directory changes)
$binaryPath = (Resolve-Path "./bin/Debug/gitz.exe").Path

# Check if binary exists
if (-not (Test-Path $binaryPath)) {
    Write-Host "❌ Build verification failed!"
    Write-Host "Binary not found: $binaryPath"
    exit 1
}

# Test help command
Write-Host "Testing help command..."
$helpResult = & $binaryPath --help 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Help command test failed!"
    Write-Host "Error: $helpResult"
    exit 1
}

# Test version command
Write-Host "Testing version command..."
$versionResult = & $binaryPath --version 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Version command test failed!"
    Write-Host "Error: $versionResult"
    exit 1
}

# Create temporary verification directory (outside Git-Z source)
$verifyDir = "$env:TEMP\verify_gitz"
if (Test-Path $verifyDir) {
    Remove-Item -Recurse -Force $verifyDir
}
New-Item -ItemType Directory -Force $verifyDir
Set-Location $verifyDir

# Test repository initialization
Write-Host "Testing repository initialization..."
& $binaryPath init 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Repository initialization test failed!"
    exit 1
}

# Verify repository structure
Write-Host "Verifying repository structure..."
$requiredDirs = @(".gitz/objects", ".gitz/refs")
$requiredFiles = @(".gitz/HEAD", ".gitz/index")

# Check directories exist
foreach ($dir in $requiredDirs) {
    if (-not (Test-Path -PathType Container $dir)) {
        Write-Host "❌ Repository structure verification failed!"
        Write-Host "Missing directory: $dir"
        exit 1
    }
}

# Check files exist
foreach ($file in $requiredFiles) {
    if (-not (Test-Path -PathType Leaf $file)) {
        Write-Host "❌ Repository structure verification failed!"
        Write-Host "Missing file: $file"
        exit 1
    }
}

# Cleanup and return to project root
Set-Location ".."
if (Test-Path $verifyDir) {
    Remove-Item -Recurse -Force $verifyDir
}

Write-Host "✓ All verification checks passed!"
Write-Host "Git-Z is ready for submission and use!""
