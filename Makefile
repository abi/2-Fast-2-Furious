grant: sccfinder2.c
	gcc -std=gnu99 -lrt -g -o sccfinder2 sccfinder2.c

random: randomgraph.cpp
	g++ -lrt -o random randomgraph.cpp


sccfinder: sccfinder.c
	gcc -lrt -o sccfinder sccfinder.c

opt: sccfinder.c
	gcc -lrt -O3 -o sccfinder sccfinder.c

debug: sccfinder.c
	gcc -lrt -g -o sccfinder sccfinder.c



clean:
	rm -f sccfinder sccfinder.o
