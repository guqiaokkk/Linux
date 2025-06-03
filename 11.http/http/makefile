.PHONY:all
all:httpserver

httpserver:ServerMain.cc 
	g++ -o $@ $^ -std=c++14 -lpthread -g



.PHONY:clean 
clean:
	rm -rf httpserver