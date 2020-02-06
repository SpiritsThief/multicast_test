
TARGET=multicast_test

$(TARGET): main.o multicast_listener.o multicast_dispatcher.o
	gcc -o multicast_test main.o multicast_listener.o multicast_dispatcher.o

multicast_listener.o: multicast_listener.c multicast_global.h
	gcc -c multicast_listener.c

multicast_dispatcher.o: multicast_dispatcher.c multicast_global.h
	gcc -c multicast_dispatcher.c

main.o: main.c multicast_global.h
	gcc -c main.c 


.PHONY: clean

clean:
	rm *.o $(TARGET)
