#include "ErrorRequestProcessor.h"
#include "HttpRequest.h"
#include "xhttpdServer.h"


xhttpdServer::xhttpdServer(uint16_t port)
: listener_()
, port_(port) {		
	
	listener_.makeNonBlocking();
	listener_.bind(Address("0.0.0.0", port));
	listener_.listen();
	
	
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
	
	Buffer<uint8_t> BUFFer;
	
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
				s -> makeNonBlocking();
				handlers_[s -> native()] = s;
				
				ev_.events = EPOLLIN | EPOLLET;
				ev_.data.fd = s -> native();
	
				int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, s -> native(), &ev_);
				if (ret != 0) {
					THROW_SYSTEM_ERROR();
				}
				
			}
			else if (events.get()[i].events & EPOLLIN) {				
				Buffer<uint8_t> buf;
				
				SocketPtr sock = handlers_[events.get()[i].data.fd];

				sock->read(buf);
				_I(buf.data());
			
				std::string requestStr(
					reinterpret_cast<const char*>(buf.data()), buf.size());

				HttpRequest request(requestStr);

				if (buf.size() == 0) {
					handlers_.erase(events.get()[i].data.fd);

					ev_.events = 0;
					ev_.data.fd = events.get()[i].data.fd;
	
					int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, events.get()[i].data.fd, &ev_);
					if (ret != 0) {
						THROW_SYSTEM_ERROR();
					}						
					
					continue;
				}
				for (auto it = processors_.begin(); it != processors_.end(); ++it) {
			
					if ((*it)->isEligible(request)) {
						Buffer<uint8_t> buffer;
		
						try {			
											
							(*it)->process(request, buffer);
							sock -> write(buffer.data(), buffer.size());
							
						} catch (std::system_error& e) {
														
							ErrorRequestProcessor ep;
							ep.setLastErrorCode(e.code().value());
							ep.process(request, buffer);		
							_E(e.what());
							
							handlers_.erase(events.get()[i].data.fd);
							continue;
						}
						
						BUFFer = buffer;

//						packFdData pfd(sock -> native(),buffer);						
//						ev_.data.ptr = (void*) &pfd;
//						packFdData *pfdptr = (packFdData *)ev_.data.ptr;

						ev_.data.ptr = (void*) &buffer;
						Buffer<uint8_t> *buffptr = (Buffer<uint8_t> *)ev_.data.ptr;
						
//						ev_.events = EPOLLOUT | EPOLLET;

//               			epoll_ctl(epfd_, EPOLL_CTL_MOD, sock -> native(), &ev_);
					}
				}
				
			}else if (events.get()[i].events & EPOLLOUT) {

//				packFdData *pfdptr = (packFdData *)events.get()[i].data.ptr;

				Buffer<uint8_t> *bufferptr = (Buffer<uint8_t> *)events.get()[i].data.ptr;
				
//				SocketPtr sock = handlers_[events.get()[i].data.fd];
//
//				sock -> write(buffer.data(), buffer.size());

//				ev_.data.fd = sock -> native();
            	ev_.events = EPOLLIN | EPOLLET;
//            	epoll_ctl(epfd_, EPOLL_CTL_MOD, sock -> native(), &ev_); 
			}
		}
	}
}



void xhttpdServer::addReqProcessor(RequestProcessorPtr processor) {
	
	processors_.push_back(processor);
}
