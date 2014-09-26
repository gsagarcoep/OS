CC=g++ 

CFLAGS=-c -g

myshell: main.o myshell.o myhistory.o myecho.o
	$(CC) main.o myshell.o myhistory.o myecho.o -o myshell

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

myshell.o: myshell.cpp
	$(CC) $(CFLAGS) myshell.cpp

myhistory.o: myhistory.cpp
	$(CC) $(CFLAGS) myhistory.cpp
	
myecho.o: myecho.cpp
	$(CC) $(CFLAGS) myecho.cpp
	
clean:
	rm -if *.o 
	
