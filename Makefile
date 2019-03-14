all: ./bin/main.out ./bin/testclient.out

./bin/main.out: ./bin/debug/main.o ./bin/debug/rsocket.o ./bin/debug/rcontainer.o ./bin/debug/rsocketcontainer.o
	gcc ./bin/debug/main.o ./bin/debug/rsocket.o ./bin/debug/rcontainer.o ./bin/debug/rsocketcontainer.o -o ./bin/main.out -pthread

./bin/testclient.out: ./bin/debug/testclient.o
	gcc ./bin/debug/testclient.o -o ./bin/testclient.out -pthread

./bin/debug/main.o: main.c
	gcc main.c -c -g -o ./bin/debug/main.o

./bin/debug/rsocket.o: rsocket.c
	gcc rsocket.c -c -g -o ./bin/debug/rsocket.o

./bin/debug/rcontainer.o: rcontainer.c
	gcc rcontainer.c -c -g -o ./bin/debug/rcontainer.o
 
./bin/debug/rsocketcontainer.o: rsocketcontainer.c
	gcc rsocketcontainer.c -c -g -o ./bin/debug/rsocketcontainer.o

./bin/debug/testclient.o: testclient.c
	gcc testclient.c -c -g -o ./bin/debug/testclient.o

clear:
	rm ./bin/main.out ./bin/testclient.out
	rm ./bin/debug/*.o 