#pragma once

#include <arpa/inet.h>
#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <sys/socket.h>

#define BUFFLEN 1024

class Balancer
{
public:
    Balancer(const std::string& filename); // Этот конструктор собирает сразу конфигурацию

    void connect();
    void listen();
    void distribute();

private:
    struct Config
    {        
        unsigned int inPort = 0;
        std::vector<unsigned int> servers{};
        unsigned int N = 0;
    } configuration;

    sockaddr_in serverAddress, clientAddress;
    char buffer[1024];
    int sockfd;

};
