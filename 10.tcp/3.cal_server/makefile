.PHONY:all
all:calserver calclient

calserver:ServerMain.cc
	g++ -o $@ $^ -std=c++14 -lpthread -ljsoncpp

calclient:ClientMain.cc
	g++ -o $@ $^ -std=c++14 -ljsoncpp

.PHONY:clean 
clean:
	rm -rf calserver calclient