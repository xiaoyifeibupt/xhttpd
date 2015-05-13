#ifndef __WORKER_H__
#define __WORKER_H__

#include <memory>
#include <thread>

#include "Poll.h"
#include "Socket.h"

class Worker
{
public:

	Worker();
	virtual ~Worker();
	void operator()();
	void addFD(SocketPtr sock, Poll::PollHandler handler);
	void start();
	size_t size() const;

private:
	Poll poll_;
	std::shared_ptr<std::thread> thread_;
};


#endif
