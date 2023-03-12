#include "balancer.h"

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string.h>
#include <chrono>
#include <thread>
#include <unistd.h>

Balancer::Balancer(const std::string& filename)
{
    Config returnConfig{};
    std::ifstream configFile;
    configFile.open(filename);
    if(!configFile.is_open())
    {
        throw std::invalid_argument("Cannot open the configuration file, check the filename!");
    }
    std::string line;
    while(getline(configFile, line))
    {
        int delim = line.find("=");
        std::string parameterName = line.substr(0, delim);
        if(delim >= 0) 
        {
            line = line.substr(delim + 1, line.size());
        }
        if(parameterName == "port")
        {
            returnConfig.inPort = atoi(line.c_str());
        }
        else if (parameterName == "servers")
        {
            size_t pos = 0;
            std::string token;
            while ((pos = line.find(", ")) != std::string::npos) 
            {
                token = line.substr(0, pos);
                returnConfig.servers.push_back(atoi(token.c_str()));
                line.erase(0, pos + 2);
            }
            returnConfig.servers.push_back(atoi(line.c_str()));
        }
        else if (parameterName == "N")
        {
            returnConfig.N = atoi(line.c_str());
        }
        else
        {
            std::cout << "There is no such parameter in the configuration! Check configuration file\n";
        }
    }
    configFile.close();
    if(returnConfig.inPort == 0 || returnConfig.servers.empty() || returnConfig.N == 0)
    {
        throw std::invalid_argument("The configuration file was invalid");
    }

    configuration = returnConfig;
}

void Balancer::connect()
{
    
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        throw std::runtime_error("failed to create a socket");
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));
    serverAddress.sin_family    = AF_INET; // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(configuration.inPort);

    if ( bind(sockfd, (const struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0 )
    {
        throw std::runtime_error("failed to bind server socket");
    }

}

void Balancer::distribute()
{
    socklen_t socketLength;
    socketLength = sizeof(clientAddress);
    int recieveLength;

    std::vector<sockaddr_in> servers;
    for(int i = 0; i < configuration.servers.size(); i++)
    {
        sockaddr_in temp;
        temp.sin_port = configuration.servers[i];
        temp.sin_addr.s_addr = INADDR_ANY; 
        temp.sin_family = AF_INET;
        servers.push_back(temp);
    }
    
    std::queue<std::chrono::time_point<std::chrono::system_clock>> recieveTime;

    size_t serverIterator = 0;

    using namespace std::chrono_literals;
    while (true)
    {
        
        while(recieveTime.size() > 0 && ((std::chrono::system_clock::now() - recieveTime.front())/1ms > 1000)) // если самая старая запись времени была больше секунды назад, мы ее удаляем, а также удаляем все записи со времени больше секунды
        {
            recieveTime.pop();
            std::cout << "recievetime size: " << recieveTime.size() << '\n';
        }
        if(recieveTime.size() <= configuration.N)
        {
            std::cout << "waiting for data...\n";            
            if((recieveLength = recvfrom(sockfd, buffer, BUFFLEN, 0, (struct sockaddr*) &clientAddress, &socketLength)) < 0)
            {
                throw std::runtime_error("bad reception");
            }
            buffer[recieveLength] = '\0';
            recieveTime.push(std::chrono::system_clock::now());
            std::cout << "Recieved packet from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "\n";
            std::cout << "data: " << buffer;
            std::cout << "since latest data: " << abs(recieveTime.front() - std::chrono::system_clock::now())/1ms << '\n';
            std::cout << "Iterator = " << serverIterator << '\n';

            if(sendto(sockfd, buffer, recieveLength, 0, (const sockaddr*)&servers[serverIterator], sizeof(serverAddress)) < 0)
            {
                throw std::runtime_error("bad sending");
            }

            std::cout << "sent packet to " << inet_ntoa(servers[serverIterator].sin_addr) << ":" << ntohs(servers[serverIterator].sin_port) << "\n";

            serverIterator++;
            serverIterator %= configuration.servers.size();
        }        
    }    
    
}
