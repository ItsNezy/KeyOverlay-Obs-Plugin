#include "wsserver.hpp"
#include <App.h>
#include <obs-module.h>

WsServer::WsServer() {}

WsServer::~WsServer() {
    stop();
}

bool WsServer::start(int port) {
    if (running_) return false;
    running_ = true;
    serverThread_ = std::thread(&WsServer::serverLoop, this, port);
    return true;
}

void WsServer::stop() {
    if (!running_) return;

    running_ = false;
    if (loop_) {
        // defer a callback to stop the loop from the thread it's running on
        loop_->defer([this]() {
            if (listenSocket_) {
                us_listen_socket_close(0, listenSocket_);
                listenSocket_ = nullptr;
            }
            std::lock_guard<std::mutex> lock(clientsMutex_);
            for (auto* ws : clientsList_) {
                ws->close();
            }
            clientsList_.clear();
            clients_ = 0;
        });
    }

    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

void WsServer::broadcast(const std::string& json) {
    if (!running_ || !loop_) return;
    
    // allocate a copy of the string to pass into the lambda
    std::string* jsonCopy = new std::string(json);
    
    loop_->defer([this, jsonCopy]() {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        for (auto* ws : clientsList_) {
            ws->send(*jsonCopy, uWS::OpCode::TEXT);
        }
        delete jsonCopy;
    });
}

int WsServer::clientCount() const {
    return clients_;
}

bool WsServer::isRunning() const {
    return running_;
}

void WsServer::serverLoop(int port) {
    // save the thread's loop instance
    loop_ = uWS::Loop::get();

    uWS::App().ws<int>("/*", {
        .compression = uWS::DISABLED,
        .maxPayloadLength = 16 * 1024,
        .idleTimeout = 120,
        .maxBackpressure = 1 * 1024 * 1024,
        .closeOnBackpressureLimit = false,
        .resetIdleTimeoutOnSend = false,
        .sendPingsAutomatically = true,
        
        .upgrade = nullptr,
        
        .open = [this](auto* ws) {
            std::lock_guard<std::mutex> lock(clientsMutex_);
            clientsList_.push_back(ws);
            clients_++;
            blog(LOG_INFO, "[KeyOverlay] WebSocket client connected. Total: %d", clients_.load());
        },
        
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            // we don't expect messages from the overlay
        },
        
        .drain = [](auto* ws) {
            // check ws->getBufferedAmount() here
        },
        
        .ping = [](auto* ws, std::string_view) {},
        
        .pong = [](auto* ws, std::string_view) {},
        
        .close = [this](auto* ws, int code, std::string_view message) {
            std::lock_guard<std::mutex> lock(clientsMutex_);
            auto it = std::find(clientsList_.begin(), clientsList_.end(), ws);
            if (it != clientsList_.end()) {
                clientsList_.erase(it);
                clients_--;
                blog(LOG_INFO, "[KeyOverlay] WebSocket client disconnected. Total: %d", clients_.load());
            }
        }
    }).listen(port, [port, this](auto* listen_socket) {
        if (listen_socket) {
            listenSocket_ = listen_socket;
            blog(LOG_INFO, "[KeyOverlay] WebSocket server running on ws://127.0.0.1:%d", port);
        } else {
            blog(LOG_ERROR, "[KeyOverlay] Failed to start WebSocket server on port %d. Is the port in use?", port);
            running_ = false;
        }
    }).run();
    
    // once loop ends
    blog(LOG_INFO, "[KeyOverlay] WebSocket server stopped");
    loop_ = nullptr;
    running_ = false;
}
