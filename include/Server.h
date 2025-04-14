#pragma once

#include <Winsock2.h>

#include <thread>
#include <vector>

class Server
{
  public:
    Server();
    Server(addrinfo info);

    virtual ~Server();

    void Run();

    int getThreadPoolSize();
    void setThreadPoolSize();

  private:
    void CleanUp();

    addrinfo _info;

    std::vector<std::thread> _threadPool;
    size_t _threadPoolSize;
};