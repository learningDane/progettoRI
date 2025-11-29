# compiler
CC = clang
CFLAGS = -Wall -I$(INCLUDEPATH)
LDFLAGS =

# paths
OBJPATH = objects
LIBPATH = libs
INCLUDEPATH = include

# source files
LAVAGNA_SRC = lavagna.c $(LIBPATH)/lib.c
UTENTE_SRC  = utente.c  $(LIBPATH)/lib.c

# object files
LAVAGNA_OBJ = $(LAVAGNA_SRC:%.c=$(OBJPATH)/%.o)
UTENTE_OBJ  = $(UTENTE_SRC:%.c=$(OBJPATH)/%.o)

# executables
LAVAGNA_EXECUTABLE_NAME = lavagna
UTENTE_EXECUTABLE_NAME  = utente

# build both
all: $(LAVAGNA_EXECUTABLE_NAME) $(UTENTE_EXECUTABLE_NAME)

# linking
$(LAVAGNA_EXECUTABLE_NAME): $(LAVAGNA_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(UTENTE_EXECUTABLE_NAME): $(UTENTE_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# object build rule
$(OBJPATH)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJPATH)/*.o $(LAVAGNA_EXECUTABLE_NAME) $(UTENTE_EXECUTABLE_NAME)
