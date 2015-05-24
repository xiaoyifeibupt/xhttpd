#include <limits.h>

#include "xhttpdServer.h"


xhttpdServer::xhttpdServer(uint16_t port, size_t threads)
: listener_()
, port_(port) 
,xhttpdWorker_(threads) {
	
	listener_.bind(Address("0.0.0.0", port));
	listener_.listen();
	
	for(size_t i = 0; i < threads; ++i) {
		xhttpdWorker_[i].start();
	}	
	
		
}

xhttpdServer::~xhttpdServer() {

}

void xhttpdServer::start() {	
	
	_I("Server started at 0.0.0.0:" << port_);

	while(1) {
		SocketPtr socket = listener_.accept();
		socket->makeNonBlocking();

		size_t min = INT_MAX;
		size_t choice = 0;
		for(size_t i = 0; i < xhttpdWorker_.size(); ++i) {
			if(xhttpdWorker_[i].size() < min) {
				choice = i;
				min = xhttpdWorker_[i].size();
			}
		}

		xhttpdWorker_[choice].addEvent(socket);

	}
}

