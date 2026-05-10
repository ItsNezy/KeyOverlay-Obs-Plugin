#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

namespace uWS {
    template <bool isSSL> struct TemplatedApp;
    using App = TemplatedApp<false>;
    struct Loop;
    template <bool isSSL, bool isServer> struct WebSocket;
}

struct us_listen_socket_t;

class WsServer {
public:
    WsServer();
    ~WsServer();

    bool start(int port = 9001);
    void stop();
    void broadcast(const std::string& json);
    int clientCount() const;
    bool isRunning() const;

private:
    void serverLoop(int port);

    std::thread serverThread_;
    std::atomic<bool> running_ = false;
    std::atomic<int> clients_ = 0;
    
    uWS::Loop* loop_ = nullptr;
    us_listen_socket_t* listenSocket_ = nullptr;
    std::mutex clientsMutex_;
    std::vector<uWS::WebSocket<false, true>*> clientsList_;
};
