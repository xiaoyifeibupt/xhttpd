CF=-std=c++11 -Wall -Wextra -pthread -g

xhttpd : main.o FS.o FSRequestProcessor.o ErrorRequestProcessor.o HttpRequest.o Poll.o Server.o Worker.o Socket.o
	g++ -o xhttpd $(CF) main.o FS.o FSRequestProcessor.o ErrorRequestProcessor.o HttpRequest.o Poll.o Server.o Worker.o Socket.o

main.o : main.cpp FSRequestProcessor.h Server.h
	g++ -c $(CF) main.cpp

FS.o : FS.cpp FS.h 
	g++ -c $(CF) FS.cpp

FSRequestProcessor.o : FSRequestProcessor.cpp FSRequestProcessor.h FS.h
	g++ -c $(CF) FSRequestProcessor.cpp

HttpRequest.o : HttpRequest.cpp HttpRequest.h
	g++ -c $(CF) HttpRequest.cpp

ErrorRequestProcessor.o : ErrorRequestProcessor.cpp ErrorRequestProcessor.h
	g++ -c $(CF) ErrorRequestProcessor.cpp

Poll.o : Poll.cpp Poll.h Log.h
	g++ -c $(CF) Poll.cpp

Server.o : Server.cpp Server.h ErrorRequestProcessor.h HttpRequest.h
	g++ -c $(CF) Server.cpp

Worker.o : Worker.cpp Worker.h
	g++ -c $(CF) Worker.cpp

Socket.o : Socket.cpp Socket.h Log.h
	g++ -c $(CF) Socket.cpp
