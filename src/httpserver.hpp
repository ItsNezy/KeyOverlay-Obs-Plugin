#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <winsock2.h>

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    bool start(int port = 9000);
    void stop();

private:
    void serverLoop(int port);
    void handleClient(SOCKET clientSock);

    std::thread serverThread_;
    std::atomic<bool> running_{false};
    SOCKET listenSocket_ = INVALID_SOCKET;
};
