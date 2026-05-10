#include "wsserver.hpp"
#include <obs-module.h>
#include <ws2tcpip.h>
#include <wincrypt.h>
#include <regex>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "crypt32.lib")

WsServer::WsServer() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

WsServer::~WsServer() {
    stop();
    WSACleanup();
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

    if (serverThread_.joinable()) {
        serverThread_.join();
    }

    std::lock_guard<std::mutex> lock(clientsMutex_);
    if (listenSocket_ != INVALID_SOCKET) {
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }
    for (auto& c : clients_) {
        if (c.sock != INVALID_SOCKET) {
            closesocket(c.sock);
        }
    }
    clients_.clear();
    clientsCount_ = 0;
}

void WsServer::broadcast(const std::string& json) {
    if (!running_) return;
    
    std::string frame;
    frame.push_back((char)0x81); // FIN + Text
    if (json.size() <= 125) {
        frame.push_back((char)json.size());
    } else if (json.size() <= 65535) {
        frame.push_back((char)126);
        frame.push_back((char)((json.size() >> 8) & 0xFF));
        frame.push_back((char)(json.size() & 0xFF));
    } else {
        return; // Too large for our simple server
    }
    frame += json;

    std::lock_guard<std::mutex> lock(clientsMutex_);
    for (auto& c : clients_) {
        if (c.handshaked && c.sock != INVALID_SOCKET) {
            send(c.sock, frame.c_str(), (int)frame.size(), 0);
        }
    }
}

int WsServer::clientCount() const {
    return clientsCount_;
}

bool WsServer::isRunning() const {
    return running_;
}

bool WsServer::doHandshake(Client& client) {
    std::string req = client.buffer;
    
    std::regex keyRegex("Sec-WebSocket-Key:\\s*(.*?)\r\n", std::regex_constants::icase);
    std::smatch match;
    if (!std::regex_search(req, match, keyRegex)) return false;
    
    std::string key = match[1].str();
    
    // Trim trailing whitespace or CR just in case
    while (!key.empty() && (key.back() == '\r' || key.back() == ' ')) {
        key.pop_back();
    }
    
    std::string concat = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    std::vector<BYTE> hash(20);
    
    if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {
            if (CryptHashData(hHash, (const BYTE*)concat.data(), (DWORD)concat.size(), 0)) {
                DWORD hashLen = 20;
                CryptGetHashParam(hHash, HP_HASHVAL, hash.data(), &hashLen, 0);
            }
            CryptDestroyHash(hHash);
        }
        CryptReleaseContext(hProv, 0);
    }
    
    DWORD outLen = 0;
    CryptBinaryToStringA(hash.data(), (DWORD)hash.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &outLen);
    std::string base64Hash(outLen, '\0');
    CryptBinaryToStringA(hash.data(), (DWORD)hash.size(), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &base64Hash[0], &outLen);
    
    // CryptBinaryToStringA includes the null terminator in outLen, so resize to drop it
    if (!base64Hash.empty() && base64Hash.back() == '\0') {
        base64Hash.pop_back();
    }
    
    std::string res = "HTTP/1.1 101 Switching Protocols\r\n"
                      "Upgrade: websocket\r\n"
                      "Connection: Upgrade\r\n"
                      "Sec-WebSocket-Accept: " + base64Hash + "\r\n\r\n";
                      
    send(client.sock, res.c_str(), (int)res.size(), 0);
    return true;
}

void WsServer::serverLoop(int port) {
    listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket_ == INVALID_SOCKET) {
        blog(LOG_ERROR, "[KeyOverlay] Failed to create socket");
        running_ = false;
        return;
    }

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(listenSocket_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        blog(LOG_ERROR, "[KeyOverlay] Failed to bind to port %d", port);
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
        running_ = false;
        return;
    }

    if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR) {
        blog(LOG_ERROR, "[KeyOverlay] Failed to listen");
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
        running_ = false;
        return;
    }

    u_long mode = 1;
    ioctlsocket(listenSocket_, FIONBIO, &mode);
    
    blog(LOG_INFO, "[KeyOverlay] WebSocket server running on ws://127.0.0.1:%d", port);

    while (running_) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listenSocket_, &readfds);
        
        std::unique_lock<std::mutex> lock(clientsMutex_);
        for (auto& c : clients_) {
            if (c.sock != INVALID_SOCKET) {
                FD_SET(c.sock, &readfds);
            }
        }
        lock.unlock();

        timeval tv = {0, 100000}; // 100ms
        int ret = select(0, &readfds, NULL, NULL, &tv);
        
        if (ret > 0) {
            lock.lock();
            if (FD_ISSET(listenSocket_, &readfds)) {
                SOCKET newClient = accept(listenSocket_, NULL, NULL);
                if (newClient != INVALID_SOCKET) {
                    u_long clientMode = 1;
                    ioctlsocket(newClient, FIONBIO, &clientMode);
                    clients_.push_back({newClient, false, ""});
                }
            }

            for (auto it = clients_.begin(); it != clients_.end(); ) {
                if (it->sock != INVALID_SOCKET && FD_ISSET(it->sock, &readfds)) {
                    char buf[2048];
                    int len = recv(it->sock, buf, sizeof(buf), 0);
                    if (len <= 0) {
                        closesocket(it->sock);
                        it = clients_.erase(it);
                        continue;
                    }
                    if (!it->handshaked) {
                        it->buffer.append(buf, len);
                        if (it->buffer.find("\r\n\r\n") != std::string::npos) {
                            if (doHandshake(*it)) {
                                it->handshaked = true;
                                it->buffer.clear(); // free memory
                                blog(LOG_INFO, "[KeyOverlay] WebSocket client connected");
                            } else {
                                closesocket(it->sock);
                                it = clients_.erase(it);
                                continue;
                            }
                        }
                    } else {
                        // ignore incoming websocket frames, just drain the buffer
                        // typical browsers send PONGs or close frames, which we just read and drop
                    }
                }
                ++it;
            }
            
            int activeClients = 0;
            for (auto& c : clients_) if (c.handshaked) activeClients++;
            if (activeClients != clientsCount_) {
                clientsCount_ = activeClients;
                blog(LOG_INFO, "[KeyOverlay] WebSocket active clients: %d", clientsCount_.load());
            }
            lock.unlock();
        }
    }
    
    blog(LOG_INFO, "[KeyOverlay] WebSocket server stopped");
}
