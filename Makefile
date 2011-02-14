
sccfinder: sccfinder.c
	gcc -lrt -o sccfinder sccfinder.c

clean:
	rm -f sccfinder sccfinder.o
