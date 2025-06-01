.PHONY:all
all:udpserver udpclient

udpserver:UdpServerMain.cc
	g++ -o $@ $^ -std=c++14 -lpthread

udpclient:UdpClientMain.cc
	g++ -o $@ $^ -std=c++14 -lpthread

.PHONY:clean
clean:
	rm -rf udpserver udpclient