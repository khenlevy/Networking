/*
 * UDPSocket.cpp
 */

#include "UDPSocket.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

UDPSocket::UDPSocket(int port){
	/**
	 * int socket(int domain, int type, int protocol);
	 * creates a UDP socket
	 * AF_INET - IPv4 Internet protocols
	 * SOCK_DGRAM - UDP
	 * 0 - default protocol type fo UDP
	 */

	socket_fd = socket (AF_INET, SOCK_DGRAM, 0);

	// Clear the s_in struct - some odd hack
	bzero((char *) &s_in, sizeof(s_in));

	// Sets the sin address
	s_in.sin_family = (short)AF_INET;
	s_in.sin_addr.s_addr = htonl(INADDR_ANY);
	s_in.sin_port = htons((u_short)port);

	fsize = sizeof(from);

	if(bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in))<0){
		perror ("Error naming channel");
	}
}

int UDPSocket::recv(char* buffer, int length){
	return recvfrom(socket_fd,buffer,length,0,(struct sockaddr *)&from,&fsize);
}

int UDPSocket::sendTo(string msg, string ip, int port){
	struct sockaddr_in toAddr;
	bzero((char *) &toAddr, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
	toAddr.sin_addr.s_addr = inet_addr(ip.data());
	toAddr.sin_port = htons(port);
	return sendto(socket_fd,msg.data(),msg.length(),0,(struct sockaddr *)&toAddr,sizeof(toAddr));
}

int UDPSocket::reply(string msg){
	return sendto(socket_fd,msg.data(),msg.length(),0,(struct sockaddr *)&from,sizeof(from));
}

void UDPSocket::cclose(){
	cout<<"closing socket"<<endl;
	shutdown(socket_fd,SHUT_RDWR);
	close(socket_fd);
}

string UDPSocket::fromAddr(){
	return inet_ntoa(from.sin_addr);
}
