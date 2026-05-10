#pragma once

#include <windows.h>
#include <string>
#include <functional>
#include <thread>
#include <atomic>

class KeyHook {
public:
    struct KeyEvent {
        std::string type;      // "keydown" or "keyup"
        int keyCode;           // virtual key code (Windows VK_ code)
        std::string key;       // human-readable name, e.g. "A", "Space", "ShiftLeft"
        long long timestamp;   // milliseconds since epoch
    };

    using EventCallback = std::function<void(const KeyEvent&)>;

    KeyHook();
    ~KeyHook();

    bool start(EventCallback callback);
    void stop();
    bool isRunning() const;

private:
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    void messageLoop();
    std::string keyCodeToName(int vkCode);

    HHOOK hook_ = nullptr;
    std::thread hookThread_;
    std::atomic<bool> running_ = false;
    EventCallback callback_;
    static KeyHook* instance_; // needed for static callback
};
