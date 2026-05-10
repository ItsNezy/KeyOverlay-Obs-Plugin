# deploy-local.ps1 — Deploy KeyOverlay plugin to local OBS installation
# Run as Administrator!

param(
    [string]$Config = "RelWithDebInfo",
    [string]$BuildDir = "build_x64"
)

$ErrorActionPreference = 'Stop'

# Find OBS install path
$OBSPath = $null
$regPath = Get-ItemProperty -Path "HKLM:\SOFTWARE\OBS Studio" -ErrorAction SilentlyContinue
if ($regPath) {
    $OBSPath = $regPath.'(default)'
}
if (-not $OBSPath) {
    $regPath = Get-ItemProperty -Path "HKCU:\SOFTWARE\OBS Studio" -ErrorAction SilentlyContinue
    if ($regPath) {
        $OBSPath = $regPath.'(default)'
    }
}
if (-not $OBSPath) {
    if (Test-Path "${env:ProgramFiles}\obs-studio") {
        $OBSPath = "${env:ProgramFiles}\obs-studio"
    } else {
        Write-Error "OBS Studio not found! Install OBS first."
        exit 1
    }
}

Write-Host "OBS Studio found at: $OBSPath" -ForegroundColor Green

# Find DLL
$dllSource = $null
# Try release dir first (after cmake --install)
$releaseDll = Get-ChildItem -Path "release" -Recurse -Filter "keyoverlay.dll" -ErrorAction SilentlyContinue | Select-Object -First 1
if ($releaseDll) {
    $dllSource = $releaseDll.FullName
} else {
    # Try build dir
    $buildDll = Get-ChildItem -Path $BuildDir -Recurse -Filter "keyoverlay.dll" -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($buildDll) {
        $dllSource = $buildDll.FullName
    }
}

if (-not $dllSource) {
    Write-Error "keyoverlay.dll not found in release/ or $BuildDir/. Build the plugin first!"
    exit 1
}

Write-Host "DLL source: $dllSource" -ForegroundColor Cyan

# Copy DLL
$dllDest = Join-Path $OBSPath "obs-plugins\64bit"
if (-not (Test-Path $dllDest)) {
    New-Item -ItemType Directory -Force -Path $dllDest | Out-Null
}
Copy-Item $dllSource "$dllDest\keyoverlay.dll" -Force
Write-Host "  -> Copied DLL to: $dllDest\keyoverlay.dll" -ForegroundColor Green

# Copy data
$dataDest = Join-Path $OBSPath "data\obs-plugins\keyoverlay"
if (-not (Test-Path $dataDest)) {
    New-Item -ItemType Directory -Force -Path $dataDest | Out-Null
}

# Try release data first
$releaseData = Get-ChildItem -Path "release" -Recurse -Directory -Filter "data" -ErrorAction SilentlyContinue | Select-Object -First 1
if ($releaseData) {
    Copy-Item -Path "$($releaseData.FullName)\*" -Destination $dataDest -Recurse -Force
    Write-Host "  -> Copied data from: $($releaseData.FullName)" -ForegroundColor Green
} elseif (Test-Path "data") {
    Copy-Item -Path "data\*" -Destination $dataDest -Recurse -Force
    Write-Host "  -> Copied data from: data/" -ForegroundColor Green
} else {
    Write-Warning "No data/ directory found. Plugin may not have web UI."
}

Write-Host ""
Write-Host "=== KeyOverlay deployed successfully ===" -ForegroundColor Green
Write-Host "Restart OBS Studio to load the plugin." -ForegroundColor Yellow
