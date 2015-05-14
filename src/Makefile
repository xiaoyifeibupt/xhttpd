CF=-std=c++11 -Wall -Wextra -pthread -g

xhttpd : main.o FS.o FSRequestProcessor.o ErrorRequestProcessor.o HttpRequest.o xhttpdServer.o Socket.o
	g++ -o xhttpd $(CF) main.o FS.o FSRequestProcessor.o ErrorRequestProcessor.o HttpRequest.o xhttpdServer.o Socket.o

main.o : main.cpp FSRequestProcessor.h xhttpdServer.h
	g++ -c $(CF) main.cpp

FS.o : FS.cpp FS.h 
	g++ -c $(CF) FS.cpp

FSRequestProcessor.o : FSRequestProcessor.cpp FSRequestProcessor.h FS.h
	g++ -c $(CF) FSRequestProcessor.cpp

HttpRequest.o : HttpRequest.cpp HttpRequest.h
	g++ -c $(CF) HttpRequest.cpp

ErrorRequestProcessor.o : ErrorRequestProcessor.cpp ErrorRequestProcessor.h
	g++ -c $(CF) ErrorRequestProcessor.cpp

xhttpdServer.o : xhttpdServer.cpp xhttpdServer.h ErrorRequestProcessor.h HttpRequest.h
	g++ -c $(CF) xhttpdServer.cpp

Socket.o : Socket.cpp Socket.h Log.h
	g++ -c $(CF) Socket.cpp
