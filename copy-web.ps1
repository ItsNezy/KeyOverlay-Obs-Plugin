# Run this as admin to update the web UI directly
Copy-Item -Path "web\*" -Destination "C:\Program Files\obs-studio\data\obs-plugins\keyoverlay\ui\" -Recurse -Force
Write-Host "Web UI updated successfully!" -ForegroundColor Green
