CC = g++
OBJS = sched.o process.o main.o

scehd.o: sched.h sched.cpp
	$(CC) -c sched.cpp

process.o: process.h process.cpp
	$(CC) -c process.cpp

main.o: main.cpp sched.h process.h
	$(CC) -c main.cpp

main: $(OBJS) 
	$(CC) $(OBJS) -o main

clean :
	rm -f $(OBJS) main



