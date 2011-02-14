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
