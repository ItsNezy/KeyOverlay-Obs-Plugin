#include "keyhook.hpp"
#include <chrono>
#include <obs-module.h>

KeyHook* KeyHook::instance_ = nullptr;

KeyHook::KeyHook() {}

KeyHook::~KeyHook() {
    stop();
}

bool KeyHook::start(EventCallback callback) {
    if (running_) return false;

    callback_ = callback;
    instance_ = this;
    running_ = true;

    // Start hook thread
    hookThread_ = std::thread(&KeyHook::messageLoop, this);
    return true;
}

void KeyHook::stop() {
    if (!running_) return;

    running_ = false;
    if (hook_) {
        UnhookWindowsHookEx(hook_);
        hook_ = nullptr;
    }

    if (hookThread_.joinable()) {
        // Send quit message to thread
        DWORD threadId = GetThreadId(hookThread_.native_handle());
        if (threadId != 0) {
            PostThreadMessage(threadId, WM_QUIT, 0, 0);
        }
        hookThread_.join();
    }
    
    instance_ = nullptr;
}

bool KeyHook::isRunning() const {
    return running_;
}

void KeyHook::messageLoop() {
    hook_ = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
    if (!hook_) {
        blog(LOG_ERROR, "[KeyOverlay] Failed to install keyboard hook");
        running_ = false;
        return;
    }

    blog(LOG_INFO, "[KeyOverlay] Keyboard hook installed");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Clean up if not already stopped
    if (hook_) {
        UnhookWindowsHookEx(hook_);
        hook_ = nullptr;
    }
}

LRESULT CALLBACK KeyHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && instance_ && instance_->callback_) {
        auto* kbdStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (isKeyDown || isKeyUp) {
            KeyEvent event;
            event.type = isKeyDown ? "keydown" : "keyup";
            event.keyCode = kbdStruct->vkCode;
            event.key = instance_->keyCodeToName(kbdStruct->vkCode);
            
            auto now = std::chrono::system_clock::now();
            event.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count();

            instance_->callback_(event);
        }
    }
    
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

std::string KeyHook::keyCodeToName(int vkCode) {
    // A-Z
    if (vkCode >= 0x41 && vkCode <= 0x5A) {
        return "Key" + std::string(1, (char)vkCode);
    }
    // 0-9
    if (vkCode >= 0x30 && vkCode <= 0x39) {
        return "Digit" + std::string(1, (char)vkCode);
    }
    // F1-F12
    if (vkCode >= 0x70 && vkCode <= 0x7B) {
        return "F" + std::to_string(vkCode - 0x70 + 1);
    }
    // Numpad0-9
    if (vkCode >= 0x60 && vkCode <= 0x69) {
        return "Numpad" + std::to_string(vkCode - 0x60);
    }

    switch (vkCode) {
        case VK_SPACE: return "Space";
        case VK_RETURN: return "Enter";
        case VK_BACK: return "Backspace";
        case VK_TAB: return "Tab";
        case VK_ESCAPE: return "Escape";
        case VK_LSHIFT: return "ShiftLeft";
        case VK_RSHIFT: return "ShiftRight";
        case VK_LCONTROL: return "ControlLeft";
        case VK_RCONTROL: return "ControlRight";
        case VK_LMENU: return "AltLeft";
        case VK_RMENU: return "AltRight";
        case VK_LWIN: return "MetaLeft";
        case VK_RWIN: return "MetaRight";
        case VK_CAPITAL: return "CapsLock";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case VK_HOME: return "Home";
        case VK_END: return "End";
        case VK_PRIOR: return "PageUp";
        case VK_NEXT: return "PageDown";
        case VK_UP: return "ArrowUp";
        case VK_DOWN: return "ArrowDown";
        case VK_LEFT: return "ArrowLeft";
        case VK_RIGHT: return "ArrowRight";
        case VK_OEM_MINUS: return "Minus";
        case VK_OEM_PLUS: return "Equal";
        case VK_OEM_4: return "BracketLeft";
        case VK_OEM_6: return "BracketRight";
        case VK_OEM_5: return "Backslash";
        case VK_OEM_1: return "Semicolon";
        case VK_OEM_7: return "Quote";
        case VK_OEM_COMMA: return "Comma";
        case VK_OEM_PERIOD: return "Period";
        case VK_OEM_2: return "Slash";
        case VK_OEM_3: return "Backquote";
        case VK_MULTIPLY: return "NumpadMultiply";
        case VK_ADD: return "NumpadAdd";
        case VK_SUBTRACT: return "NumpadSubtract";
        case VK_DECIMAL: return "NumpadDecimal";
        case VK_DIVIDE: return "NumpadDivide";
        case VK_NUMLOCK: return "NumLock";
        case VK_SNAPSHOT: return "PrintScreen";
        case VK_SCROLL: return "ScrollLock";
        case VK_PAUSE: return "Pause";
        default: return "Key_" + std::to_string(vkCode);
    }
}
