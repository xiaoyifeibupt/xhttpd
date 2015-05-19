#include <system_error>

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
	
	Buffer<uint8_t> responseStr;
	
	_I("Server started at 0.0.0.0:" << port_);
	
	auto events = UNIQ_MEM_PTR(struct epoll_event, kMaxEvents, free);

	while (1) {
		int nfds = epoll_wait(epfd_, events.get(), kMaxEvents, -1);

		if (nfds == -1 && errno != EINTR) {			
			THROW_SYSTEM_ERROR();
		}

		for (int i = 0; i < nfds; ++i) {
			
			if(events.get()[i].data.fd == listener_.native()) {
				_I("acceptbefore");

				int socketfd;
				while((socketfd = listener_.accept()) > 0) {
					_I("accept");
					Socket sock(socketfd);
					sock.makeNonBlocking();
					
					ev_.events = EPOLLIN | EPOLLET;
					ev_.data.fd = socketfd;
		
					int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, socketfd, &ev_);
					if (ret != 0) {
						THROW_SYSTEM_ERROR();
					}							
				}
				if(socketfd == -1) {
					if (errno != EAGAIN && errno != ECONNABORTED 
                            && errno != EPROTO && errno != EINTR)
						THROW_SYSTEM_ERROR();
				}
				continue;
			}
			else if (events.get()[i].events & EPOLLIN) {
				_I("in");				
				Buffer<uint8_t> buf;
				
				Socket sock(events.get()[i].data.fd);

//				sock.read(buf);
				
			
				std::string requestStr(
					reinterpret_cast<const char*>(buf.data()), buf.size());

				HttpRequest request(requestStr);

				if (!(request.method == HttpRequest::Method::GET || request.method == HttpRequest::Method::POST)) {
					
					ev_.events = 0;
					ev_.data.fd = events.get()[i].data.fd;
	
					int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, events.get()[i].data.fd, &ev_);
					if (ret != 0) {
						THROW_SYSTEM_ERROR();
					}						
					sock.close(events.get()[i].data.fd);
					
					continue;
				}

				_I(buf.data());
				for (auto it = processors_.begin(); it != processors_.end(); ++it) {
			
					if ((*it)->isEligible(request)) {
						Buffer<uint8_t> buffer;
		
						try {			
											
								(*it)->process(request, buffer);

								packFdData *pfdptr = (packFdData*)malloc(sizeof(packFdData));
								pfdptr -> fdno = events.get()[i].data.fd;
								pfdptr -> bu_data = buffer.data();
								pfdptr -> bu_size = buffer.size();

								ev_.data.ptr = (void*) pfdptr;
								ev_.events = EPOLLOUT | EPOLLET;

								int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, events.get()[i].data.fd, &ev_);
								if (ret != 0) {
									THROW_SYSTEM_ERROR();
								}
								
							} 
						catch (std::system_error& e) {
														
							ErrorRequestProcessor ep;
							ep.setLastErrorCode(e.code().value());
							ep.process(request, buffer);

							if(buffer.size() > 0)
								sock.write(buffer.data(), buffer.size());		
							_E(e.what());
							
							ev_.events = 0;
							ev_.data.fd = events.get()[i].data.fd;
	
							int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, events.get()[i].data.fd, &ev_);
							if (ret != 0) {
								THROW_SYSTEM_ERROR();
							}						
							
							sock.close(events.get()[i].data.fd);
							continue;
						}						
					}
				}
				
			}else if (events.get()[i].events & EPOLLOUT) {
				_I("out");

				packFdData *pfdptr = (packFdData*)malloc(sizeof(packFdData));
				pfdptr = (packFdData*)events.get()[i].data.ptr;
				
				Buffer<uint8_t> responseStr;
				responseStr.append(pfdptr -> bu_data,pfdptr -> bu_size);

				Socket sock(pfdptr -> fdno);

				if(responseStr.size() > 0)
					sock.write(responseStr.data(), responseStr.size());
				sock.close(pfdptr -> fdno);
/*
				ev_.data.fd = pfdptr -> fdno;
            	ev_.events = EPOLLIN | EPOLLET;
            	int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, pfdptr -> fdno, &ev_);
            	if (ret != 0) {
					THROW_SYSTEM_ERROR();
				}


				ev_.events = 0;
				ev_.data.fd = sock -> native();
	
				int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, sock -> native(), &ev_);
				if (ret != 0) {
					THROW_SYSTEM_ERROR();
				}						
				
				sock -> close(sock -> native());
				continue;
*/			
			}
		}
	}
}



void xhttpdServer::addReqProcessor(RequestProcessorPtr processor) {
	
	processors_.push_back(processor);
}
