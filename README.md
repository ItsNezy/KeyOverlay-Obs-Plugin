<div align="center">

# ⌨ KeyOverlay

**A lightweight, native OBS plugin that displays a real-time keyboard overlay on your stream.**

Built with C++ and raw Winsock2 — zero external dependencies, maximum performance.

[![Build](https://img.shields.io/github/actions/workflow/status/ItsNezy/KeyOverlay-Obs-Plugin/build.yml?style=flat-square&logo=github)](https://github.com/ItsNezy/KeyOverlay-Obs-Plugin/actions)
[![Release](https://img.shields.io/github/v/release/ItsNezy/KeyOverlay-Obs-Plugin?style=flat-square&color=8B5CF6)](https://github.com/ItsNezy/KeyOverlay-Obs-Plugin/releases/latest)
[![License](https://img.shields.io/github/license/ItsNezy/KeyOverlay-Obs-Plugin?style=flat-square)](LICENSE)

<!-- Replace with actual demo GIF -->
<!-- ![KeyOverlay Demo](docs/demo.gif) -->

</div>

---

## ✨ Features

- 🎮 **Global Key Capture** — Uses a low-level Windows keyboard hook to capture keystrokes from *any* application, including fullscreen games
- ⚡ **Zero Dependencies** — No Node.js, no Python, no external libraries. Pure C++ with native Winsock2 networking
- 🎨 **8 Built-in Themes** — Dark, Light, Gaming Red, Cyberpunk, Ocean, Pastel, Minimal, Monochrome
- 🔧 **Visual Key Editor** — Click any key to resize, reposition, or relabel. Drag-and-drop support
- 📐 **6 Keyboard Layouts** — Full (104-key), TKL (87-key), 75%, 65%, 60%, Numpad
- 🌐 **Browser Source Integration** — Works as an OBS Browser Source with a single URL
- 🔄 **Auto Update Checker** — Get notified when a new version is available
- 💎 **Glassmorphism Effects** — Premium frosted-glass backdrop with neon glow animations

## 📥 Installation

### Option 1: Installer (Recommended)
1. Go to the [**Releases**](https://github.com/ItsNezy/KeyOverlay-Obs-Plugin/releases/latest) page
2. Download `keyoverlay-installer.exe`
3. Run the installer — it will auto-detect your OBS installation
4. Restart OBS Studio

### Option 2: Manual
1. Download the `.zip` from [Releases](https://github.com/ItsNezy/KeyOverlay-Obs-Plugin/releases/latest)
2. Extract `keyoverlay.dll` → `C:\Program Files\obs-studio\obs-plugins\64bit\`
3. Extract `data\` folder → `C:\Program Files\obs-studio\data\obs-plugins\keyoverlay\`
4. Restart OBS Studio

## 🚀 Usage

### 1. Open the Editor
After installing, open your browser and go to:
```
http://127.0.0.1:9000
```
This is the visual editor where you can customize your overlay.

### 2. Customize
- Choose a **theme** and **layout**
- Adjust the **scale** and **position**
- Click individual keys to edit their width, label, or position
- Use custom colors to match your stream aesthetic

### 3. Add to OBS
1. Click **📋 Copy OBS URL** in the editor
2. In OBS, add a new **Browser** source
3. **Paste** the copied URL
4. Set Width: `1200`, Height: `400`
5. Check **"Refresh browser when scene becomes active"**
6. Done! Your keyboard overlay is live 🎉

## 🏗 Architecture

```
┌─────────────────────────────────────────┐
│  OBS Studio                             │
│  ┌────────────────────────────────────┐ │
│  │  keyoverlay.dll (C++ Plugin)       │ │
│  │  ├─ KeyHook     (WH_KEYBOARD_LL)  │ │
│  │  ├─ WsServer    (ws://9001)       │ │
│  │  ├─ HttpServer  (http://9000)     │ │
│  │  └─ DockPanel   (Qt Widgets)      │ │
│  └────────────────────────────────────┘ │
│                                         │
│  ┌────────────────────────────────────┐ │
│  │  Browser Source                    │ │
│  │  └─ Connects to ws://127.0.0.1:9001│
│  │     and renders keyboard overlay   │ │
│  └────────────────────────────────────┘ │
└─────────────────────────────────────────┘
```

- **KeyHook** — Low-level Windows keyboard hook captures all keystrokes globally
- **WsServer** — Raw RFC 6455 WebSocket server broadcasts key events to connected clients
- **HttpServer** — Serves the web UI (editor + overlay) from the plugin's data directory
- **DockPanel** — Qt-based settings panel integrated into OBS's dock system

## 🛠 Building from Source

### Prerequisites
- Windows 10/11
- Visual Studio 2022
- CMake 3.16+
- OBS Studio 30.x+ source/deps

### Build
```powershell
# Configure
cmake --preset windows-x64

# Build
cmake --build build_x64 --config RelWithDebInfo --parallel

# Deploy locally (run as admin)
.\deploy-local.ps1
```

## 📄 License

This project is open source. See [LICENSE](LICENSE) for details.

---

<div align="center">
Made with ❤ for streamers
</div>
