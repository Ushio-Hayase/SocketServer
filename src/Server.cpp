#include "Server.h"

Server::Server()
    : maxThreadPoolSize_(DEFAULT_THREADPOOL_SIZE),
      maxBufferSize_(DEFAULT_SOCKET_RECV_BUFFER_SIZE),
      isRunning_(false)
{
    threadPool_.resize(DEFAULT_THREADPOOL_SIZE);
    clientSockets_.resize(DEFAULT_THREADPOOL_SIZE);
}

void Server::Run(std::string port)
{
    isRunning_ = true;

    addrinfo initaddr;

    int resultValue =
        WSAStartup(MAKEWORD(2, 2), &wsaData_);  // 2.2버전 winsock 호출

    if (resultValue != 0)
    {
        std::cerr << "WSAStartup failed, Error code : " << resultValue;
        Stop();
        return;
    }

    memset(&initaddr, 0, sizeof(initaddr));
    initaddr.ai_family = AF_INET;
    initaddr.ai_socktype = SOCK_STREAM;
    initaddr.ai_protocol = IPPROTO_TCP;
    initaddr.ai_flags = AI_PASSIVE;

    resultValue = getaddrinfo(nullptr, port.c_str(), &initaddr, &addr_);
    if (resultValue != 0)
    {
        std::cerr << "getaddrinfo failed, Error code : " << resultValue;
        freeaddrinfo(addr_);
        Stop();
        return;
    }

    listenSocket =
        socket(addr_->ai_family, addr_->ai_socktype, addr_->ai_protocol);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "socket failed, Error code : " << WSAGetLastError();
        freeaddrinfo(addr_);
        CleanUp(listenSocket);
        Stop();
        return;
    }

    freeaddrinfo(addr_);

    resultValue = listen(listenSocket, SOMAXCONN);
    if (resultValue == SOCKET_ERROR)
    {
        std::cerr << "listen failed, Error code : " << WSAGetLastError();
        CleanUp(listenSocket);
        Stop();
    }

    Accept();
}

void Server::Stop()
{
    WSACleanup();
}

void Server::CleanUp(SOCKET socket)
{
    closesocket(socket);
}

void Server::Accept()
{
    SOCKET newSocket = accept(listenSocket, nullptr, nullptr);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (threadPool_.size() > maxThreadPoolSize_)
        {
            CleanUp(newSocket);
            return;
        }

        threadPool_.emplace_back(ClientHandler, this, newSocket);
        threadPool_.back().detach();
        clientSockets_.push_back(newSocket);
    }
}

void Server::ClientHandler(SOCKET clientSocket)
{
    char* buffer = new char[maxBufferSize_];
    size_t recvBytes;

    do
    {
        memset(buffer, 0, maxBufferSize_);
        recvBytes = recv(clientSocket, buffer, maxBufferSize_, 0);

        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& sock : clientSockets_)
        {
            if (sock != clientSocket)
            {
                send(sock, buffer, recvBytes, 0);
            }
        }

    } while (recvBytes > 0);

    closesocket(clientSocket);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        clientSockets_.erase(std::remove(clientSockets_.begin(),
                                         clientSockets_.end(), clientSocket),
                             clientSockets_.end());
    }
}

int main()
{
    Server server;

    server.Run("8888");
}