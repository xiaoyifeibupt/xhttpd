#include <limits>

#include "ErrorRequestProcessor.h"
#include "HttpRequest.h"
#include "xhttpdServer.h"


xhttpdServer::xhttpdServer(uint16_t port)
: listener_()
, port_(port) {		
	
	listener_.bind(Address("0.0.0.0", port));
	listener_.listen();
	listener_.makeNonBlocking();
	
	ev_.events = EPOLLIN | EPOLLET;
	ev_.data.fd = listener_.native();
	
	epfd_ = epoll_create(100000);
	
	int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, listener_.native(), &ev_);
	
	if (ret != 0) {
		THROW_SYSTEM_ERROR();
	}
		
}

xhttpdServer::~xhttpdServer() {

}

void xhttpdServer::start() {
	
	_I("Server started at 0.0.0.0:" << port_);
	
	auto events = UNIQ_MEM_PTR(struct epoll_event, kMaxEvents, free);

	while (1) {
		int nfds = epoll_wait(epfd_, events.get(), kMaxEvents, -1);

		if (nfds == -1 && errno != EINTR) {			
			THROW_SYSTEM_ERROR();
		}

		for (int i = 0; i < nfds; ++i) {
			
			if(events.get()[i].data.fd == listener_.native()) {
				SocketPtr s = listener_.accept();
				
				handlers_[s -> native()] = s;
				
				ev_.events = EPOLLIN | EPOLLET;
				ev_.data.fd = s -> native();
	
				int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, s -> native(), &ev_);
				if (ret != 0) {
					THROW_SYSTEM_ERROR();
				}
			}
			else if (events.get()[i].events & EPOLLIN) {
				if (handler(handlers_[events.get()[i].data.fd], 0) < 0) {						
					handlers_.erase(events.get()[i].data.fd);
					continue;
				}
			}else if (events.get()[i].events & EPOLLOUT) {
				if (handler(handlers_[events.get()[i].data.fd], 1) < 0) {						
					handlers_.erase(events.get()[i].data.fd);
					continue;
				}
			}
		}
	}
}

int xhttpdServer::handler(SocketPtr sock, int x) {
	
	if (x == 0) {
		
		Buffer<uint8_t> buf;

		sock->read(buf);
		
		std::string requestStr(
			reinterpret_cast<const char*>(buf.data()), buf.size());

		HttpRequest request(requestStr);

		if (buf.size() == 0)
			return -1;

		for (auto it = processors_.begin(); it != processors_.end(); ++it) {
			
			if ((*it)->isEligible(request)) {

				try {
					return (*it)->process(request, sock);
				} catch (std::system_error& e) {
					
					
					ErrorRequestProcessor ep;
					ep.setLastErrorCode(e.code().value());
					ep.process(request, sock);

					_E(e.what());
					return -1;
				}
			}
		}

		return -1;
	}
	else if (x == 1) {
		
		if (sock->needToWrite() && sock->write() == 0)
			return -1;
	}

	return 0;
}

void xhttpdServer::addReqProcessor(RequestProcessorPtr processor) {
	
	processors_.push_back(processor);
}
