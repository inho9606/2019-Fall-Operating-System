OBJECTS = main.o list.o hash.o bitmap.o
SRC = main.c list.c hash.c bitmap.c

CC = gcc
CPPFLAGS = -g -c
TARGET= testlib
$(TARGET):$(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)
clean:
	rm $(OBJECTS) $(TARGET)