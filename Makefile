CXXFLAGS = -std=c++11 -Werror -Wall -g -I. -pedantic
CXX = g++

run: main.o
	$(CXX) $(CXXFLAGS) -o xcut main.o -lpthread

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f main.o xcut
	

