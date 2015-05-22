#include <iostream>

#include "NormalRequestProcessor.h"
#include "xhttpdServer.h"

int main()
{
	xhttpdServerPtr server = std::make_shared<xhttpdServer>(8080);


	server->addReqProcessor(
		std::make_shared<NormalRequestProcessor>());

	server->start();

	return 0;
}
