# Makefile for dog transportation problem base code
# Sharlee Climer


CC      = g++
CFLAGS  = -g
TARGET  = bruteForce
OBJS    = dogTrans.o

$(TARGET):	$(OBJS)
		$(CC) -o $(TARGET) $(OBJS)

dogTrans.o:	dogTrans.cpp dogTrans.h timer.h
		$(CC) $(CFLAGS) -c dogTrans.cpp

clean:
		/bin/rm -f *.o $(TARGET)
