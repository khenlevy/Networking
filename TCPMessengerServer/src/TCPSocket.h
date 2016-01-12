/*
 * TCPSocket.h
 */

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

class TCPSocket{
private:
	struct sockaddr_in serverAddr;
	struct sockaddr_in peerAddr;
	int socket_fd;

	// Private constructor to create a secondary server socket to communicate with a remote peer
	TCPSocket(int connected_sock,struct sockaddr_in serverAddr,struct sockaddr_in peerAddr);

	// Read from socket into the given buffer up to the buffer given length. Return the number of bytes read
	int recv(char* buffer, int length);

	// Send the given buffer to the socket
	int send(const char* msg, int len);

public:

	// Constructor create a TCP server socket
	TCPSocket(int port);

	// Constructor creates TCP client socket
	TCPSocket(string peerIp, int port);

	// Perform listen and accept on server socket
	TCPSocket* listenAndAccept();

	// Close the socket and free all resources
	void cclose();

	// Return the address of the connected peer
	string fromAddr();

	string destIpAndPort();

	// Returns the c socket fid
	int getSocketFid();

	// Communication commands
	int readCommand();
	string readMsg();
	void writeMsg(string msg);
	void writeCommand(int command);
};

#endif
