CC = gcc
CPP = g++
CFLAGS = -std=gnu99
LDFLAGS = 
SOURCES = randomgraph.cpp sccfinder.c sccfinder2.c
OBJECTS=$(SOURCES:.cpp=.o)

#TODO: What exactly these flags are supposed to do
ifneq (Darwin, $(shell uname))
	CFLAGS += -lrt
endif

#all : $(SOURCES) $(BINS)

#$(BINS):
#	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

random: randomgraph.cpp
	$(CPP) $(CFLAGS) -o random randomgraph.cpp

boostscc: boostscc.cpp
	$(CPP) $(CFLAGS) -o boost boostscc.cpp


threads: threads.c
	$(CC) $(CFLAGS) -o threads threads.c

sccfinder2: sccfinder2.c
	$(CC) $(CFLAGS) -g -o sccfinder2 sccfinder2.c

scc: scc.c
	$(CC) $(CFLAGS) -O3 -o sccfinder scc.c

opt: sccfinder.c
	$(CC) $(CFLAGS) -O3 -o scc scc.c

debug: sccfinder.c
	$(CC) $(CFLAGS) -g -o sccfinder sccfinder.c


sccfinder: scc.c
	$(CC) $(CFLAGS) -O3 -lpthread -o sccfinder scc.c

scc: scc.c
	$(CC) $(CFLAGS) -O3 -lpthread -o sccfinder scc.c

clean:
	rm -rf sccfinder random *.o *.out *.output sccfinder2
	rm -rf sccfinder2.dSYM scc scc.dSYM dual-core bench-string bench-utils
