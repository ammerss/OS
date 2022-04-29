CC = g++
OBJS = main.o

main.o : main.cpp
	$(CC) -c main.cpp
main : $(OBJS)
	$(CC) $(OBJS) -o main
clean :
	rm -f $(OBJS) main
