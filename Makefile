CC = clang
CFLAGS = -Wall
OBJPATH = objects
LDFLAGS =

all: $(OBJECTS)

%.o : %.c
	$(CC) -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(OBJ)
