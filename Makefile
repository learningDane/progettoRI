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

# dependencies
$(OBJPATH)/lavagna.o: include/include.h include/include_lavagna.h libs/lib_lavagna.c libs/lib.c
$(OBJPATH)/utente.o:  include/include.h include/include_utente.h libs/lib_utente.c libs/lib.c
$(OBJPATH)/libs/lib.o: include/include.h

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

# compilare la documentazione (necessita di una installazione latex)
documentazione: documentazione/documentazione.tex
	cd documentazione && pdflatex documentazione.tex && mv documentazione.pdf ./pdfoutput/ && cd ..

clean:
	rm -rf $(OBJPATH)/* $(LAVAGNA_EXECUTABLE_NAME) $(UTENTE_EXECUTABLE_NAME)
