all:frequency

frequency: frequency.o
	gcc -Wall frequency.o -o frequency

frequency.o: frequency.c
	gcc -Wall -c frequency.c -o frequency.o

.PHONY: clean all

clean: 
	rm *.o -f frequency