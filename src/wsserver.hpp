#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <winsock2.h>

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
    struct Client {
        SOCKET sock;
        bool handshaked;
        std::string buffer;
    };

    void serverLoop(int port);
    bool doHandshake(Client& client);

    std::thread serverThread_;
    std::atomic<bool> running_ = false;
    std::atomic<int> clientsCount_ = 0;
    
    SOCKET listenSocket_ = INVALID_SOCKET;
    std::mutex clientsMutex_;
    std::vector<Client> clients_;
};
