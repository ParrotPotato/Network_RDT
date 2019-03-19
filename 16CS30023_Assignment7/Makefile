all: librsocket.a

librsocket.a : rsocket.o
	ar cr librsocket.a rsocket.o

rsocket.o : rsocket.c
	gcc rsocket.c -c -o rsocket.o -w 

clear:
	rm *.a *.o *.out