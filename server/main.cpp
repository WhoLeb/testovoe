/*
	Simple udp server
*/
#include<stdio.h>	//printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <stdexcept>
#include <iostream>

#define BUFFLEN 512	//Max length of buffer
#define PORT 9000	//The port on which to listen for incoming data

void die(char *s)
{
	perror(s);
	exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd;

    char buffer[BUFFLEN];

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        throw std::runtime_error("failed to create a socket");
    }
    
    sockaddr_in balancerAddress, clientAddress;

    memset(&balancerAddress, 0, sizeof(balancerAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));
    balancerAddress.sin_family    = AF_INET; // IPv4
    balancerAddress.sin_addr.s_addr = INADDR_ANY;
    balancerAddress.sin_port = htons(atoi(argv[1]));

    if ( bind(sockfd, (const struct sockaddr*)&balancerAddress, sizeof(balancerAddress)) < 0 )
    {
        throw std::runtime_error("failed to bind server socket");
    }
	
    socklen_t socketLength;
    socketLength = sizeof(clientAddress);
    int recieveLength;

    std::cout << "server address " << inet_ntoa(balancerAddress.sin_addr) << ":" << ntohs(balancerAddress.sin_port) << "\n";

	//keep listening for data
	while(1)
	{
		std::cout << "waiting for data...\n";            
            if((recieveLength = recvfrom(sockfd, buffer, BUFFLEN, 0, (struct sockaddr*) &clientAddress, &socketLength)) < 0)
            {
                throw std::runtime_error("bad reception");
            }
            buffer[recieveLength] = '\0';
            std::cout << "Recieved packet from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "\n";
            std::cout << "data: " << buffer;
	}

	return 0;
}