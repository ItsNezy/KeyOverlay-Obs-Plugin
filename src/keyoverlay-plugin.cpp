#include "keyoverlay-plugin.hpp"
#include <obs-module.h>

KeyOverlayPlugin& KeyOverlayPlugin::getInstance() {
    static KeyOverlayPlugin instance;
    return instance;
}

bool KeyOverlayPlugin::init() {
    keyHook_ = std::make_unique<KeyHook>();
    wsServer_ = std::make_unique<WsServer>();
    httpServer_ = std::make_unique<HttpServer>();
    
    keyHook_->start([this](const KeyHook::KeyEvent& event) {
        std::string json = "{\"type\":\"" + event.type + "\",\"keyCode\":" + 
            std::to_string(event.keyCode) + ",\"key\":\"" + event.key + 
            "\",\"timestamp\":" + std::to_string(event.timestamp) + "}";
            
        if (wsServer_->isRunning()) {
            wsServer_->broadcast(json);
        }
    });

    if (!wsServer_->start(9001)) {
        blog(LOG_ERROR, "[KeyOverlay] Failed to start WebSocket server");
    }

    if (!httpServer_->start(9000)) {
        blog(LOG_ERROR, "[KeyOverlay] Failed to start HTTP server");
    }

    dockPanel_ = std::make_unique<DockPanel>(wsServer_.get());
    dockPanel_->init();

    return true;
}

void KeyOverlayPlugin::shutdown() {
    if (keyHook_) {
        keyHook_->stop();
        keyHook_.reset();
    }
    if (wsServer_) {
        wsServer_->stop();
        wsServer_.reset();
    }
    if (httpServer_) {
        httpServer_->stop();
        httpServer_.reset();
    }
    if (dockPanel_) {
        dockPanel_.reset();
    }
}
