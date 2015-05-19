#ifndef __XHTTPDSERVER_H__
#define __XHTTPDSERVER_H__

#include <sys/epoll.h>
#include <memory>
#include <thread>
#include <map>

#include "Socket.h"
#include "RequestProcessor.h"
#include "Buffer.h"


static const size_t kMaxEvents = 1 << 16;

template<typename T, typename D>
static std::unique_ptr<T, D> __memBlock(size_t size, D deleter) {
	
	return std::unique_ptr<T, D>(reinterpret_cast<T*>(
		calloc(size, sizeof(T))), deleter);
}

#define UNIQ_MEM_PTR(T, size, D) __memBlock<T, decltype(&D)>(size, &D)

typedef struct packFD {

	int fdno;
	const uint8_t *bu_data;
	size_t bu_size;

}packFdData;


class xhttpdServer : public std::enable_shared_from_this<xhttpdServer>
{
public:

	xhttpdServer(uint16_t port);
	
	virtual ~xhttpdServer();
	
	void start();
	
	void addReqProcessor(RequestProcessorPtr processor);

private:

	Socket listener_;
	
	uint16_t port_;	
	
	std::vector<RequestProcessorPtr> processors_;

	struct epoll_event ev_;
	
	int epfd_;		
	
};
typedef std::shared_ptr<xhttpdServer> xhttpdServerPtr;

#endif