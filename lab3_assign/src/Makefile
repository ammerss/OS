CC = g++
OBJS = main.o pager.o

main.o : main.cpp fte.h pte.h pager.h
	$(CC) -c main.cpp
pager.o : pager.cpp pager.h fte.h pte.h 
	$(CC) -c pager.cpp 
main : $(OBJS) 
	$(CC) $(OBJS) -o main
clean :
	rm -f $(OBJS) main
