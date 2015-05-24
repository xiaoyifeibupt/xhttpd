#ifndef __XHTTPDSERVER_H__
#define __XHTTPDSERVER_H__

#include <memory>

#include "Socket.h"
#include "xhttpdWorker.h"


class xhttpdServer : public std::enable_shared_from_this<xhttpdServer>
{
public:

	xhttpdServer(uint16_t port, size_t threads);
	
	virtual ~xhttpdServer();
	
	void start();

private:

	Socket listener_;
	
	uint16_t port_;

	std::vector<xhttpdWorker> xhttpdWorker_;
	
};
typedef std::shared_ptr<xhttpdServer> xhttpdServerPtr;

#endif