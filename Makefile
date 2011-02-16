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

sccfinder: sccfinder.c
	$(CC) $(CFLAGS) -o sccfinder sccfinder.c

sccfinder2: sccfinder2.c
	$(CC) $(CFLAGS) -g -o sccfinder2 sccfinder2.c

scc: scc.c
	$(CC) $(CFLAGS) -g -o scc scc.c

opt: sccfinder.c
	$(CC) $(CFLAGS) -O3 -o scc scc.c

debug: sccfinder.c
	$(CC) $(CFLAGS) -g -o sccfinder sccfinder.c

clean:
	rm -rf sccfinder random *.o *.out *.output sccfinder2 sccfinder2.dSYM scc scc.dSYM dual-core
