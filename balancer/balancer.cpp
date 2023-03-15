#include "balancer.h"

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string.h>
#include <chrono>
#include <thread>
#include <unistd.h>

#define DEBUG 0

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
                int column = token.find(":");

                sockaddr_in temp;
                inet_pton(AF_INET, token.substr(0, column).c_str(), &temp.sin_addr);
                temp.sin_port = htons(atoi(token.substr(column+1, token.size()).c_str()));
                temp.sin_family = AF_INET;


                returnConfig.servers.push_back(temp);
                line.erase(0, pos + 2);
            }
            int column = token.find(":");
            sockaddr_in temp;
            inet_pton(AF_INET, line.substr(0, column).c_str(), &temp.sin_addr);
            temp.sin_port = htons(atoi(line.substr(column+1, line.size()).c_str()));
            temp.sin_family = AF_INET;

            returnConfig.servers.push_back(temp);
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

    memset(&balancerAddress, 0, sizeof(balancerAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));
    balancerAddress.sin_family    = AF_INET; // IPv4
    balancerAddress.sin_addr.s_addr = INADDR_ANY;
    balancerAddress.sin_port = htons(configuration.inPort);

    if ( bind(sockfd, (const struct sockaddr*)&balancerAddress, sizeof(balancerAddress)) < 0 )
    {
        throw std::runtime_error("failed to bind server socket");
    }

}

void Balancer::distribute()
{
    socklen_t socketLength;
    socketLength = sizeof(clientAddress);
    int recieveLength;

    
    
    std::queue<std::chrono::time_point<std::chrono::system_clock>> recieveTime;

    size_t serverIterator = 0;

    unsigned int packagesSent = 0;

    using namespace std::chrono_literals;
    while (true)
    {
        if((recieveLength = recvfrom(sockfd, buffer, BUFFLEN, 0, (struct sockaddr*) &clientAddress, &socketLength)) < 0)
        {
            throw std::runtime_error("bad reception");
        }
        buffer[recieveLength] = '\0';

        
    #if DEBUG
        std::cout << "Latest message was " << (std::chrono::system_clock::now() - recieveTime.front())/1ms << " milliseconds ago\n";
        std::cout << recieveTime.size() << " packages recieved last second\n";
    #endif

        if(packagesSent <= configuration.N)
        {
            recieveTime.push(std::chrono::system_clock::now());
            if(sendto(sockfd, buffer, recieveLength, 0, (const sockaddr*)&configuration.servers[serverIterator], sizeof(balancerAddress)) < 0)
            {
                throw std::runtime_error("bad sending");
            }
            packagesSent++;
            serverIterator++;
            serverIterator %= configuration.servers.size();
        #if DEBUG
            std::cout << "Sent message to server " << serverIterator << '\n';
            std::cout << packagesSent << " packages were sent in the last second\n";
            std::cout << "\n\n";
        #endif

        }  

        while(recieveTime.size() > 0 && ((std::chrono::system_clock::now() - recieveTime.front())/1ms > 1000)) // если самая старая запись времени была больше секунды назад, мы ее удаляем, а также удаляем все записи со времени больше секунды
        {
            recieveTime.pop();
            packagesSent--;
        }
    }    
    
}
