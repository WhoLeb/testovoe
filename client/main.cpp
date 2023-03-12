/*
	Simple udp client
*/
#include<stdio.h>	//printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <string>
#include <thread>
#include<chrono>


#define SERVER "127.0.0.1"
#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to send data

void die(std::string s)
{
	perror(s.c_str());
	exit(1);
}

int main(void)
{
	struct sockaddr_in si_other;
	int s, i;
    socklen_t slen=sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN] = "hello";

	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	
	if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
    int counter = 0;
	while(1)
	{		
        counter++;
        std::string tempstr = "hello " + std::to_string(counter) + "\n";
        strcpy(message, tempstr.c_str());
		//send the message
		if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
	}

	return 0;
}
