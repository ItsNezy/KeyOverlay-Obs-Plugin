# Run this as Administrator
$obsPluginDir = "C:\Program Files\obs-studio\obs-plugins\64bit"
$obsDataDir = "C:\Program Files\obs-studio\data\obs-plugins"

Write-Host "Uninstalling KeyOverlay Plugin..."

# Remove DLL
$dllPath = Join-Path $obsPluginDir "keyoverlay.dll"
if (Test-Path $dllPath) {
    Remove-Item -Path $dllPath -Force
    Write-Host "Removed: $dllPath" -ForegroundColor Green
} else {
    Write-Host "Not found: $dllPath" -ForegroundColor Yellow
}

# Remove Data directory
$dataDirPath = Join-Path $obsDataDir "keyoverlay"
if (Test-Path $dataDirPath) {
    Remove-Item -Path $dataDirPath -Recurse -Force
    Write-Host "Removed: $dataDirPath" -ForegroundColor Green
} else {
    Write-Host "Not found: $dataDirPath" -ForegroundColor Yellow
}

Write-Host "Uninstallation complete. You can now test the installer."
