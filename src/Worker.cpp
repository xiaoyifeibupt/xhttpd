#include "Worker.h"

Worker::Worker() : poll_(100000) {

}

Worker::~Worker() {

}

void Worker::operator()() {
	poll_.dispatch();
}
	
void Worker::addFD(SocketPtr sock, Poll::PollHandler handler) {
	poll_.addFD(sock, handler);
}

void Worker::start() {
	thread_ = std::make_shared<std::thread>(
		std::bind(&Worker::operator(), this));
}

size_t Worker::size() const {
	return poll_.size();
}
