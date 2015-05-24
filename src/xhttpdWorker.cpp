#include <system_error>

#include "xhttpdWorker.h"
#include "DataBuffer.h"
#include "ErrorRequestProcessor.h"
#include "NormalRequestProcessor.h"
#include "HttpRequest.h"



xhttpdWorker::xhttpdWorker() {
	epfd_ = epoll_create(100000);
}

xhttpdWorker::~xhttpdWorker() {

}


void xhttpdWorker::start() {
	thread_ = std::make_shared<std::thread>(
		std::bind(&xhttpdWorker::EventLoop, this));
}

void xhttpdWorker::addEvent(SocketPtr sock) {
	{
		std::unique_lock<std::mutex> lock(mutex_);
		sockMap[sock->native()] = sock;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = sock -> native();	
	
	int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, sock -> native(), &ev);
	
	if (ret != 0) {
		THROW_SYSTEM_ERROR();
	}
}

void xhttpdWorker::EventLoop() {
	
	auto events = UNIQ_MEM_PTR(struct epoll_event, kMaxEvents, free);

	while (1) {
		int nfds = epoll_wait(epfd_, events.get(), kMaxEvents, -1);

		if (nfds == -1 && errno != EINTR) {			
			THROW_SYSTEM_ERROR();
		}

		for (int i = 0; i < nfds; ++i) {
		{
			std::unique_lock<std::mutex> lock(mutex_);

//			if(sockMap.count(events.get()[i].data.fd) == 0)
//				continue;

			if (events.get()[i].events & EPOLLIN) {
								
				DataBuffer<uint8_t> buf(1024);
				
				SocketPtr socket = sockMap[events.get()[i].data.fd];

				socket -> read(buf);

				if(buf.size() == 0) {
					sockMap.erase(events.get()[i].data.fd);
					continue;
				}
							
				std::string requestStr(
					reinterpret_cast<const char*>(buf.data()), buf.size());

				HttpRequest request(requestStr);

				if (!(request.method == HttpRequest::Method::GET || request.method == HttpRequest::Method::POST)) {
					
					struct epoll_event ev_;
					ev_.events = 0;
					ev_.data.fd = events.get()[i].data.fd;
	
					int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, socket -> native(), &ev_);
					if (ret != 0) {
						THROW_SYSTEM_ERROR();
					}
					sockMap.erase(events.get()[i].data.fd);						
					socket -> close(events.get()[i].data.fd);					
					continue;
				}

				_I(buf.data());

				NormalRequestProcessor NRP;
			
				if (NRP.isEligible(request)) {
					DataBuffer<uint8_t> buffer(1024);
	
					try {			
										
							NRP.process(request, buffer);

							packFdData *pfdptr = (packFdData*)malloc(sizeof(packFdData));
							pfdptr -> fdno = events.get()[i].data.fd;
							pfdptr -> bu_data = buffer.data();
							pfdptr -> bu_size = buffer.size();

							struct epoll_event ev_;

							ev_.data.ptr = (void*) pfdptr;
							ev_.events = EPOLLOUT | EPOLLET;

							int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, socket -> native(), &ev_);
							if (ret != 0) {
								THROW_SYSTEM_ERROR();
							}
							
						} 
					catch (std::system_error& e) {
													
						ErrorRequestProcessor ep;
						ep.setLastErrorCode(e.code().value());
						ep.process(request, buffer);

						if(buffer.size() > 0)
							socket -> write(buffer.data(), buffer.size());		
						_E(e.what());
						
						struct epoll_event ev_;
						ev_.events = 0;
						ev_.data.fd = events.get()[i].data.fd;

						int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, socket -> native(), &ev_);
						if (ret != 0) {
							THROW_SYSTEM_ERROR();
						}						
						sockMap.erase(socket -> native());	
						socket -> close(socket -> native());
						continue;
					}						
				}
			}else if (events.get()[i].events & EPOLLOUT) {

				packFdData *pfdptr = (packFdData*)malloc(sizeof(packFdData));
				pfdptr = (packFdData*)events.get()[i].data.ptr;
				
				DataBuffer<uint8_t> responseStr(1024);
				responseStr.append(pfdptr -> bu_data,pfdptr -> bu_size);

				SocketPtr socket = sockMap[pfdptr -> fdno];

				if(responseStr.size() > 0)
					socket -> write(responseStr.data(), responseStr.size());
				sockMap.erase(pfdptr -> fdno);	
				socket -> close(pfdptr -> fdno);
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
}

size_t xhttpdWorker::size() const
{
	return sockMap.size();
}