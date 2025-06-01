.PHONY:all
all:udpserver udpclient

udpserver:UdpServerMain.cc
	g++ -o $@ $^ -std=c++14

udpclient:UdpClientMain.cc
	g++ -o $@ $^ -std=c++14

.PHONY:clean
clean:
	rm -rf udpserver udpclient