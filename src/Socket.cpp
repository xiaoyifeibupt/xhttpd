#include <netdb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "Socket.h"
#include "Log.h"

Address::Address(std::string _host, uint16_t _port) : a_host(_host), a_port(_port) {

}

Socket::Socket() : needToWrite_(false) {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		THROW_SYSTEM_ERROR(); 
	}
	int one = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));
	if (ret != 0) {
		THROW_SYSTEM_ERROR();
	}
}

Socket::Socket(Socket&& rhs) : 
		sockfd(std::move(rhs.sockfd)), 
		addr(rhs.addr), 
		host(rhs.host), 
		port(rhs.port) {
	rhs.sockfd = 0;
}

Socket::~Socket() {

}

void Socket::bind(Address address) {
	struct hostent *hosten = gethostbyname(address.a_host.c_str());

	if (!hosten) {
		const char *errMsg = hstrerror(h_errno);
		throw std::runtime_error(errMsg);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(address.a_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int ret = ::bind(sockfd, reinterpret_cast<struct sockaddr*>(&addr),sizeof(addr));
	if (ret != 0) {
		THROW_SYSTEM_ERROR();
	}
}

void Socket::listen() {
	int ret = ::listen(sockfd, 10000);

	if (ret != 0) {
		THROW_SYSTEM_ERROR();
	}
}

SocketPtr Socket::accept() {
	socklen_t size = sizeof(addr);
	int ret = ::accept(sockfd, reinterpret_cast<struct sockaddr*>(&addr), &size);

	if (ret == -1) {
		THROW_SYSTEM_ERROR();
	}

	return std::make_shared<Socket>(ret);
}

void Socket::makeNonBlocking() {
	long one = 1;

	int ret = ioctl(sockfd, FIONBIO, &one);

	if (ret == -1) {
		THROW_SYSTEM_ERROR();
	}
}

void Socket::read(Buffer<uint8_t>& buffer) {
	uint8_t buf[1024];
	int ret = 0;

	ret = recv(sockfd, buf, sizeof(buf), 0);
	while (ret > 0) {
		buffer.append(buf, ret);
		ret = recv(sockfd, buf, sizeof(buf), 0);
	}
}

void Socket::write(const uint8_t *buf, size_t size) {
	writeBuffer.append(buf, size);

//	return write();
	int ret = 0;

	while(writeBuffer.size() - ret) {
		ret = ::write(sockfd, writeBuffer.data(), writeBuffer.size());
		if (ret == -1) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				_E("Can't write to socket. Socket will be closed");
			}
		} else {
			writeBuffer.drain(ret);
		}
	}

}

size_t Socket::write() {
	int ret = 0;

	ret = ::write(sockfd, writeBuffer.data(), writeBuffer.size());
	if (ret == -1) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			_E("Can't write to socket. Socket will be closed");
		}
	} else {
		writeBuffer.drain(ret);
	}

	needToWrite_ = !!writeBuffer.size();

	return writeBuffer.size();
}

int Socket::native() {
	return sockfd;
}

bool Socket::needToWrite() const {
	return needToWrite_;
}

Socket::Socket(int fd) : sockfd(fd) {

}


bool Socket::error() {
	int optval;
	socklen_t optlen;

	optlen = sizeof(optval);
	getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, (socklen_t *)&optlen);

	return optval;
}

bool operator==(SocketPtr lhs, int rhs) {
	return lhs->sockfd == rhs;
}
