NAME=chalk-board
CLFAGS=-DDEBUG `pkg-config --cflags sdl3` -Wall -I./include/ -ggdb
LIBS=`pkg-config --libs sdl3` -ggdb
SRCFILES=$(wildcard src/*.c)
OBJFILES=$(addprefix build/, $(addsuffix .o,$(basename $(notdir $(SRCFILES)))))
CC=gcc
RM=rm

all: $(NAME)

$(NAME): $(OBJFILES)
	$(CC) $^ -o $(NAME) $(LIBS)

build/%.o: src/%.c
	$(CC) $(CLFAGS) -c $< -o $@

build/%.o: src/*/%.c
	$(CC) $(CLFAGS) -c $< -o $@

clean:
	$(RM) $(wildcard $(OBJFILES) $(NAME))
