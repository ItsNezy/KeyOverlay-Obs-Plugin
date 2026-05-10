#pragma once

#include <memory>
#include "keyhook.hpp"
#include "wsserver.hpp"
#include "httpserver.hpp"
#include "dock-panel.hpp"

class KeyOverlayPlugin {
public:
    static KeyOverlayPlugin& getInstance();

    bool init();
    void shutdown();

private:
    KeyOverlayPlugin() = default;

    std::unique_ptr<KeyHook> keyHook_;
    std::unique_ptr<WsServer> wsServer_;
    std::unique_ptr<HttpServer> httpServer_;
    std::unique_ptr<DockPanel> dockPanel_;
};
