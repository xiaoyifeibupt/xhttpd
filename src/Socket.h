#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/socket.h>
#include <arpa/inet.h>

#include <memory>
#include <string>

#include "Buffer.h"

struct Address {
	Address(std::string host, uint16_t port);
	std::string a_host;
	uint16_t a_port;
};

class Socket;

typedef std::shared_ptr<Socket> SocketPtr; //指向Socket类的智能指针

class Socket
{
public:

	Socket();
	Socket(int fd);
	Socket(Socket&&);
	virtual ~Socket();

	void bind(Address address);
	void listen();
	SocketPtr accept();
	
	void makeNonBlocking();
	
	void read(Buffer<uint8_t>& buffer);
	
	
	size_t write(const uint8_t *buf, size_t size);
	size_t write();
	
	int native();
	bool error();
	bool needToWrite() const;

	friend bool operator==(SocketPtr lhs, int rhs);

private:
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	int sockfd;
	struct sockaddr_in addr;

	std::string host;
	uint16_t port;
	Buffer<uint8_t> writeBuffer;
	bool needToWrite_;
};

#endif