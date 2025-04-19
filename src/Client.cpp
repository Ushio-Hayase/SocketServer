#include "Client.h"

void Client::Run(sockaddr_in serverAddr)
{
    int resultValue = WSAStartup(MAKEWORD(2, 2), &wsaData_);
    if (resultValue != 0)
    {
        std::cerr << "WSAStartup failed, Error code : " << resultValue;
        return;
    }

    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ == INVALID_SOCKET)
    {
        std::cerr << "socket failed, Error code " << WSAGetLastError();
        closesocket(socket_);
        return;
    }

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(0);
    local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(socket_, (struct sockaddr*)&local_addr, sizeof(local_addr));

    if (connect(socket_, reinterpret_cast<sockaddr*>(&serverAddr),
                sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "connect failed, Error code : " << WSAGetLastError();
        closesocket(socket_);
        WSACleanup();
        return;
    }

    char buffer[DEFAULT_SOCKET_RECV_BUFFER_SIZE];
    std::string input;

    while (true)
    {
        std::cout << "> ";
        std::cout.flush();
        std::cin >> input;
        if (input == "/quit")
        {
            send(socket_, "/quit", 6, 0);
            break;
        }

        if (send(socket_, input.c_str(), input.size() + 1, 0) == SOCKET_ERROR)
        {
            std::cerr << "send failed, Error code : " << WSAGetLastError()
                      << std::endl;
            break;
        }
        memset(buffer, 0, DEFAULT_SOCKET_RECV_BUFFER_SIZE);
        int recvBytes =
            recv(socket_, buffer, DEFAULT_SOCKET_RECV_BUFFER_SIZE, 0);
        if (recvBytes > 0)
        {
            std::cout << buffer << std::endl;
        }
    }

    closesocket(socket_);
    WSACleanup();
}

int main()
{
    sockaddr_in serverAddrIn;
    serverAddrIn.sin_family = AF_INET;
    serverAddrIn.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &serverAddrIn.sin_addr);

    Client client;

    client.Run(serverAddrIn);
}