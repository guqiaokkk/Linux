.PHONY:all
all:tcpserver tcpclient

tcpserver:TcpServerMain.cc
	g++ -o $@ $^ -std=c++14 -lpthread

tcpclient:TcpClientMain.cc
	g++ -o $@ $^ -std=c++14 -lpthread

.PHONY:clean
clean:
	rm -rf tcpserver tcpclient