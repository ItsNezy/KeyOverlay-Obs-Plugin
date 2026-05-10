#include "keyoverlay-plugin.hpp"
#include <obs-module.h>
#include <obs-frontend-api.h>

static void on_frontend_event(enum obs_frontend_event event, void* data) {
    if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
        auto* plugin = static_cast<KeyOverlayPlugin*>(data);
        plugin->initDock();
    }
}

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

    // Defer dock creation until OBS UI is fully loaded
    obs_frontend_add_event_callback(on_frontend_event, this);

    return true;
}

void KeyOverlayPlugin::initDock() {
    if (dockPanel_) return; // already initialized
    
    dockPanel_ = std::make_unique<DockPanel>(wsServer_.get());
    dockPanel_->init();
    blog(LOG_INFO, "[KeyOverlay] Dock panel created");
}

void KeyOverlayPlugin::shutdown() {
    obs_frontend_remove_event_callback(on_frontend_event, this);

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
