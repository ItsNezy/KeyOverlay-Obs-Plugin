<div align="center">

# ⌨ KeyOverlay

**A lightweight, native OBS plugin that displays a real-time keyboard overlay on your stream.**

[English](#-english) | [Bahasa Indonesia](#-bahasa-indonesia)

---

</div>

## 🇺🇸 English

### ✨ Features
- 🎮 **Global Key Capture** — Uses a low-level Windows keyboard hook to capture keystrokes from *any* application.
- ⚡ **Zero Dependencies** — Pure C++ and native Winsock2.
- 🎨 **8 Built-in Themes** — Dark, Light, Gaming Red, Cyberpunk, and more.
- 🔧 **Visual Key Editor** — Real-time resizing and repositioning.
- 📐 **6 Keyboard Layouts** — Full, TKL, 75%, 65%, 60%, Numpad.
- 🌐 **Browser Source Integration** — Works as an OBS Browser Source.

### 📥 Installation
1. Go to the [**Releases**](https://github.com/ItsNezy/KeyOverlay-Obs-Plugin/releases/latest) page.
2. Download `keyoverlay-installer.exe` and run it.
3. Restart OBS Studio.

### 🚀 Usage
1. Open your browser and go to `http://127.0.0.1:9000`.
2. Customize your overlay and click **📋 Copy OBS URL**.
3. In OBS, add a **Browser** source and paste the URL.

---

## 🇮🇩 Bahasa Indonesia

### ✨ Fitur Utama
- 🎮 **Global Key Capture** — Nangkep input keyboard dari aplikasi *apa aja* pake low-level hook.
- ⚡ **Zero Dependencies** — Full C++ tanpa library aneh-aneh.
- 🎨 **8 Tema Bawaan** — Banyak pilihan tema keren kayak Cyberpunk, Gaming Red, dll.
- 🔧 **Visual Key Editor** — Bisa edit ukuran ama posisi tombol langsung lewat browser.
- 📐 **6 Layout Keyboard** — Support dari Full size sampe Numpad doang.
- 🌐 **Integrasi Browser Source** — Tinggal copy URL ke OBS.

### 📥 Instalasi
1. Buka halaman [**Releases**](https://github.com/ItsNezy/KeyOverlay-Obs-Plugin/releases/latest).
2. Download `keyoverlay-installer.exe` terus install.
3. Restart OBS Studio.

### 🚀 Cara Pakai
1. Buka browser, akses `http://127.0.0.1:9000`.
2. Atur sesuka hati terus klik **📋 Copy OBS URL**.
3. Di OBS, tambah source **Browser** baru terus paste URL-nya.

---

## 🏗 Architecture / Arsitektur

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
└─────────────────────────────────────────┘
```

## 🛠 Building from Source

```powershell
cmake --preset windows-x64
cmake --build build_x64 --config RelWithDebInfo --parallel
.\deploy-local.ps1
```

## 📄 License

This project is open source. See [LICENSE](LICENSE) for details.
