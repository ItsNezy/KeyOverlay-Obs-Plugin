#include "httpserver.hpp"
#include <obs-module.h>
#include <ws2tcpip.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

HttpServer::HttpServer() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

HttpServer::~HttpServer() {
    stop();
    WSACleanup();
}

bool HttpServer::start(int port) {
    if (running_) return false;
    running_ = true;
    serverThread_ = std::thread(&HttpServer::serverLoop, this, port);
    return true;
}

void HttpServer::stop() {
    if (!running_) return;

    running_ = false;

    if (listenSocket_ != INVALID_SOCKET) {
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }

    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

std::string getMimeType(const std::string& path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos) return "application/octet-stream";
    std::string ext = path.substr(dotPos);
    for (auto& c : ext) c = tolower(c);

    if (ext == ".html") return "text/html; charset=utf-8";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".svg") return "image/svg+xml";
    if (ext == ".woff2") return "font/woff2";
    
    return "application/octet-stream";
}

void HttpServer::handleClient(SOCKET clientSock) {
    blog(LOG_INFO, "[KeyOverlay] HTTP client connected");

    DWORD timeout = 2000;
    setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    char buf[4096];
    int len = recv(clientSock, buf, sizeof(buf) - 1, 0);
    if (len <= 0) {
        closesocket(clientSock);
        blog(LOG_INFO, "[KeyOverlay] HTTP client disconnected");
        return;
    }
    buf[len] = '\0';
    std::string request(buf);
    
    size_t firstSpace = request.find(' ');
    size_t secondSpace = request.find(' ', firstSpace + 1);
    if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
        closesocket(clientSock);
        blog(LOG_INFO, "[KeyOverlay] HTTP client disconnected");
        return;
    }
    
    std::string method = request.substr(0, firstSpace);
    std::string path = request.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    
    if (method != "GET") {
        std::string response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        send(clientSock, response.c_str(), (int)response.size(), 0);
        closesocket(clientSock);
        blog(LOG_INFO, "[KeyOverlay] HTTP client disconnected");
        return;
    }
    
    if (path == "/") path = "/index.html";
    
    // Simple path traversal protection
    if (path.find("..") != std::string::npos) {
        std::string response = "HTTP/1.1 403 Forbidden\r\n\r\n";
        send(clientSock, response.c_str(), (int)response.size(), 0);
        closesocket(clientSock);
        blog(LOG_INFO, "[KeyOverlay] HTTP client disconnected");
        return;
    }
    
    // Build real path
    char* pluginDataPath = obs_module_file("ui");
    std::string realPath = std::string(pluginDataPath) + path;
    bfree(pluginDataPath);
    
    std::ifstream file(realPath, std::ios::binary);
    if (!file.is_open()) {
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(clientSock, response.c_str(), (int)response.size(), 0);
        closesocket(clientSock);
        blog(LOG_INFO, "[KeyOverlay] HTTP client disconnected");
        return;
    }
    
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::string mimeType = getMimeType(realPath);
    
    std::ostringstream responseHeader;
    responseHeader << "HTTP/1.1 200 OK\r\n"
                   << "Content-Type: " << mimeType << "\r\n"
                   << "Content-Length: " << fileSize << "\r\n"
                   << "Connection: close\r\n"
                   << "Access-Control-Allow-Origin: *\r\n\r\n";
                   
    std::string headerStr = responseHeader.str();
    send(clientSock, headerStr.c_str(), (int)headerStr.size(), 0);
    
    char fileBuf[8192];
    while (file.read(fileBuf, sizeof(fileBuf)) || file.gcount() > 0) {
        send(clientSock, fileBuf, (int)file.gcount(), 0);
    }
    
    closesocket(clientSock);
    blog(LOG_INFO, "[KeyOverlay] HTTP client disconnected");
}

void HttpServer::serverLoop(int port) {
    listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket_ == INVALID_SOCKET) {
        blog(LOG_ERROR, "[KeyOverlay] Failed to create HTTP socket");
        running_ = false;
        return;
    }

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(listenSocket_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        blog(LOG_ERROR, "[KeyOverlay] HTTP Failed to bind to port %d", port);
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
        running_ = false;
        return;
    }

    if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR) {
        blog(LOG_ERROR, "[KeyOverlay] HTTP Failed to listen");
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
        running_ = false;
        return;
    }

    blog(LOG_INFO, "[KeyOverlay] HTTP server running on http://127.0.0.1:%d", port);

    while (running_) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listenSocket_, &readfds);

        struct timeval tv = {1, 0}; // 1 second timeout

        int ret = select(0, &readfds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(listenSocket_, &readfds)) {
            SOCKET clientSock = accept(listenSocket_, NULL, NULL);
            if (clientSock != INVALID_SOCKET) {
                std::thread([this, clientSock]() {
                    handleClient(clientSock);
                }).detach();
            }
        }
    }
    
    blog(LOG_INFO, "[KeyOverlay] HTTP server stopped");
}
