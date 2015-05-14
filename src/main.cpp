#include <iostream>

#include "FSRequestProcessor.h"
#include "xhttpdServer.h"

int main()
{
	xhttpdServerPtr server = std::make_shared<xhttpdServer>(8080);


	server->addReqProcessor(
		std::make_shared<FSRequestProcessor>());

	server->start();

	return 0;
}
