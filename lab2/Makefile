CC = g++
CPP = g++
PROG = convex_hull datasets
CXXFLAGS = -Wall -std=c++11 -I gogui/libraries/cpp -I gogui/libraries/cpp/libraries/cppjson/include
LDLIBS = -lm

all: $(PROG)

convex_hull: convex_hull.o json.o

json.o: 
	g++ -c $(CXXFLAGS) $^ gogui/libraries/cpp/libraries/cppjson/json.cc -o $@ $(LDLIBS)

datasets: datasets.o predicates.o 
	g++ $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	
clean:
	rm $(PROG) *.o
	
