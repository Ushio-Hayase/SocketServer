#pragma once

#include <WS2tcpip.h>
#include <Winsock2.h>

#include <iostream>
#include <string>

constexpr int DEFAULT_SOCKET_RECV_BUFFER_SIZE = 1024;

class Client
{
   public:
    Client() = default;

    void Run(sockaddr_in);

   private:
    SOCKET socket_;
    WSADATA wsaData_;
};
