CC = g++
OBJS = main.o sched.o

main.o : main.cpp IO.h sched.h
	$(CC) -c main.cpp
sched.o : sched.cpp sched.h IO.h
	$(CC) -c sched.cpp
main : $(OBJS)
	$(CC) $(OBJS) -o main
clean :
	rm -f $(OBJS) main
