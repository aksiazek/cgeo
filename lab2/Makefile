CPP = g++
PROG = convex_hull
CXXFLAGS = -Wall -std=c++11
LDLIBS = -lm

all: $(PROG)

$(PROG): $(PROG).o predicates.o 
	g++ $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	
clean:
	rm $(PROG) *.o
	
