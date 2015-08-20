CF=-std=c++11 -Wall -Wextra -pthread -g

xhttpd : main.o FilePro.o NormalRequestProcessor.o ErrorRequestProcessor.o HttpRequest.o xhttpdServer.o xhttpdWorker.o Socket.o
	g++ -o xhttpd $(CF) main.o FilePro.o NormalRequestProcessor.o ErrorRequestProcessor.o HttpRequest.o xhttpdServer.o xhttpdWorker.o Socket.o

main.o : main.cpp NormalRequestProcessor.h xhttpdServer.h
	g++ -c $(CF) main.cpp

FilePro.o : FilePro.cpp FilePro.h 
	g++ -c $(CF) FilePro.cpp

NormalRequestProcessor.o : NormalRequestProcessor.cpp NormalRequestProcessor.h FilePro.h
	g++ -c $(CF) NormalRequestProcessor.cpp

HttpRequest.o : HttpRequest.cpp HttpRequest.h
	g++ -c $(CF) HttpRequest.cpp

ErrorRequestProcessor.o : ErrorRequestProcessor.cpp ErrorRequestProcessor.h
	g++ -c $(CF) ErrorRequestProcessor.cpp

xhttpdServer.o : xhttpdServer.cpp xhttpdServer.h
	g++ -c $(CF) xhttpdServer.cpp

xhttpdWorker.o : xhttpdWorker.cpp xhttpdWorker.h DataBuffer.h NormalRequestProcessor.h ErrorRequestProcessor.h HttpRequest.h
	g++ -c $(CF) xhttpdWorker.cpp

Socket.o : Socket.cpp Socket.h Log.h
	g++ -c $(CF) Socket.cpp

clean :
	rm *.o xhttpd