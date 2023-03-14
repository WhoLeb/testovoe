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
        std::vector<sockaddr_in> servers{};
        unsigned int N = 0;
    } configuration;

    sockaddr_in balancerAddress, clientAddress;
    char buffer[1024];
    int sockfd;

};
