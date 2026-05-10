# KeyOverlay

Global keyboard overlay for OBS.
Captures keypresses from anywhere on Windows and displays them on a browser source overlay inside OBS via a built-in WebSocket server.

## Build Requirements

- OBS Studio 30.x SDK
- CMake 3.20+
- Visual Studio 2022 with C++ workload
- Qt 6.x (same version OBS uses)
- uWebSockets v20
- Inno Setup 6 (for installer)

## Build Steps

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_PREFIX_PATH="path/to/obs-sdk;path/to/Qt6"
cmake --build build --config Release
```

## Install for Development

```bash
cmake --install build --config Release
```

## Build Installer

Open `installer/keyoverlay-installer.iss` in Inno Setup, click Compile.

## How It Works (for developers)

1. Plugin loads → `KeyHook` installs `WH_KEYBOARD_LL` on separate thread
2. User presses key → hook fires → `KeyHook` callback called
3. Callback serializes event to JSON → `WsServer` broadcasts to all clients
4. Browser Source (overlay) receives JSON via WebSocket → updates keyboard visual

## User Setup Instructions

1. Download and run `keyoverlay-installer.exe`
2. Restart OBS
3. In OBS, go to **Docks** menu → check **KeyOverlay** to show the settings panel
4. Add a Browser Source to your scene, set URL to your overlay file
5. Done — keys will now appear on your overlay while streaming
