.PHONY:all
all:mykill myprocess

mykill:test.cc
	g++ -o $@ $^ -std=c++11
myprocess:process.cc
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -f mykill