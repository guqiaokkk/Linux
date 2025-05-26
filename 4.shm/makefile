.PHONY:all
all:client server

client:client.cc
	g++ -o $@ $^ -std=c++17
server:server.cc
	g++ -o $@ $^ -std=c++17
.PHONY:clean
clean:
	rm -rf client server