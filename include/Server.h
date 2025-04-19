#pragma once

#include <WS2tcpip.h>
#include <Winsock2.h>

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

constexpr int DEFAULT_THREADPOOL_SIZE = 16;
constexpr int DEFAULT_SOCKET_RECV_BUFFER_SIZE = 1024;

class Server
{
   public:
    Server();

    void Run(std::string);
    void Stop();

   private:
    void CleanUp(SOCKET);
    void Accept();

    void ClientHandler(SOCKET);

    std::vector<std::thread> threadPool_;
    std::vector<SOCKET> clientSockets_;
    std::mutex mutex_;

    bool isRunning_;
    size_t maxBufferSize_;
    size_t maxThreadPoolSize_;

    addrinfo* addr_ = nullptr;
    SOCKET listenSocket;
    WSADATA wsaData_;

    std::string chat;
};
