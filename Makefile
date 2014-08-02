server: server.o led.o
	g++ -std=c++11 -o server server.o led.o

server.o: server.cpp
	g++ -std=c++11 -c server.cpp

led.o: led.cpp
	g++ -std=c++11 -c led.cpp

.PHONY: clean

clean:
	rm server *.o
