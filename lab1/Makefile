CPP = g++
PROG = det
CXXFLAGS = -Wall -std=c++11
LDLIBS = -lm

all: $(PROG)

$(PROG): $(PROG).o predicates.o 
	g++ $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	
clean:
	rm $(PROG) *.o
	
