
CC=g++ -Wall

CFLAGS=-c -g

functions: myls.o  
	$(CC) myls.o -o myls

main.o: myls.cpp
	$(CC) $(CFLAGS) myls.cpp

clean:
	rm -if *.o 
