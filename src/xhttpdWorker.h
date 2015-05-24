#ifndef __XHTTPDWORKER_H__
#define __XHTTPDWORKER_H__

#include <sys/epoll.h>
#include <memory>
#include <thread>
#include <mutex>
#include <map>

#include "RequestProcessor.h"
#include "Socket.h"



static const size_t kMaxEvents = 1 << 16;

template<typename T, typename D>
static std::unique_ptr<T, D> __memBlock(size_t size, D deleter)
{
	return std::unique_ptr<T, D>(reinterpret_cast<T*>(
		calloc(size, sizeof(T))), deleter);
}
#define UNIQ_MEM_PTR(T, size, D) __memBlock<T, decltype(&D)>(size, &D)

typedef struct packFD {

	int fdno;
	const uint8_t *bu_data;
	size_t bu_size;

}packFdData;

class xhttpdWorker
{
public:
	
	xhttpdWorker();

	virtual ~xhttpdWorker();
	
	void start();
	
	void addEvent(SocketPtr sock);

	void EventLoop();

	size_t size() const;

private:
	std::shared_ptr<std::thread> thread_;
	
	int epfd_;

	std::mutex mutex_;

	std::map<int,SocketPtr> sockMap;
};


#endif