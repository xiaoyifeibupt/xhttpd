# xhttpd©

`xhttpd©`是一个简单高效的http server，C++语言开发，主要是为了练习所学的一些知识：http 协议、网络编程、I/O复用，多线程以及C++ 11新特性。

##开发记录

-	2015.5.21 添加POST和CGI
-	2015.5.20 完成ApacheBench测试，性能还需要努力
-	2015.5.19 解决`ev.data.ptr`数据传递
-	2015.5.18 异常处理，404
-	2015.5.15 模块集成和调试
-	2015.5.12 完成html的解析设计和实现
-	2015.5.5 出差一周，进行了非阻塞IO的学习，epoll的使用
-	2015.5.4 完成socket接口的封装
-	2015.5.1 准备活动，关于文件读取，缓冲区，log设计和实现




##功能和特点

-	支持GET、POST
-	支持CGI
-	支持文件和目录的访问
-	非阻塞
-	epoll实现
-	支持403,404,500
-	不依赖第三方库



##性能测试

###ApacheBench测试

	./ab -n 10000 -c 500 http://10.108.163.248:8080/

####xhttpd(太弱了:( )

	This is ApacheBench, Version 2.3 <$Revision: 1638069 $>
	Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
	Licensed to The Apache Software Foundation, http://www.apache.org/
	
	Benchmarking 10.108.163.248 (be patient)
	Completed 1000 requests
	Completed 2000 requests
	Completed 3000 requests
	Completed 4000 requests
	Completed 5000 requests
	Completed 6000 requests
	Completed 7000 requests
	Completed 8000 requests
	Completed 9000 requests
	Completed 10000 requests
	Finished 10000 requests
	
	
	Server Software:
	Server Hostname:        10.108.163.248
	Server Port:            8080
	
	Document Path:          /
	Document Length:        0 bytes
	
	Concurrency Level:      500
	Time taken for tests:   1.410 seconds
	Complete requests:      10000
	Failed requests:        543
	   (Connect: 0, Receive: 0, Length: 543, Exceptions: 0)
	Non-2xx responses:      385
	Total transferred:      4970000 bytes
	HTML transferred:       236808 bytes
	Requests per second:    7093.94 [#/sec] (mean)
	Time per request:       70.483 [ms] (mean)
	Time per request:       0.141 [ms] (mean, across all concurrent requests)
	Transfer rate:          3443.05 [Kbytes/sec] received
	
	Connection Times (ms)
	              min  mean[+/-sd] median   max
	Connect:        0   17  96.3      5    1000
	Processing:     9   41  50.2     27     365
	Waiting:        7   39  50.9     24     365
	Total:         14   58 112.5     33    1363
	
	Percentage of the requests served within a certain time (ms)
	  50%     33
	  66%     40
	  75%     43
	  80%     44
	  90%    122
	  95%    205
	  98%    238
	  99%    242
	 100%   1363 (longest request)



####nginx

	This is ApacheBench, Version 2.3 <$Revision: 1638069 $>
	Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
	Licensed to The Apache Software Foundation, http://www.apache.org/
	
	Benchmarking 10.108.163.248 (be patient)
	Completed 1000 requests
	Completed 2000 requests
	Completed 3000 requests
	Completed 4000 requests
	Completed 5000 requests
	Completed 6000 requests
	Completed 7000 requests
	Completed 8000 requests
	Completed 9000 requests
	Completed 10000 requests
	Finished 10000 requests
	
	
	Server Software:        nginx/1.8.0
	Server Hostname:        10.108.163.248
	Server Port:            8081
	
	Document Path:          /
	Document Length:        612 bytes
	
	Concurrency Level:      500
	Time taken for tests:   0.658 seconds
	Complete requests:      10000
	Failed requests:        0
	Total transferred:      8440000 bytes
	HTML transferred:       6120000 bytes
	Requests per second:    15205.77 [#/sec] (mean)
	Time per request:       32.882 [ms] (mean)
	Time per request:       0.066 [ms] (mean, across all concurrent requests)
	Transfer rate:          12532.88 [Kbytes/sec] received
	
	Connection Times (ms)
	              min  mean[+/-sd] median   max
	Connect:        0    1   2.3      0      15
	Processing:     6    7   5.4      6     210
	Waiting:        5    7   5.3      6     210
	Total:          6    7   6.5      6     213
	
	Percentage of the requests served within a certain time (ms)
	  50%      6
	  66%      6
	  75%      6
	  80%      6
	  90%      6
	  95%     21
	  98%     25
	  99%     25
	 100%    213 (longest request)




