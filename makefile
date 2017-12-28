SRC = $(wildcard src/*.c) $(wildcard src/*/*.c)
INC = $(wildcard include/*.h) $(wildcard include/*/*.h)
OBJ = $(subst src/,obj/, $(SRC:.c=.o))
OBJDIR = $(sort $(dir $(OBJ)))

all : main

main : $(OBJ)
	cc $(OBJ) -o $@

obj/%.o : src/%.c $(INC) | $(OBJDIR)
	cc -c $<  -I include -o $@

$(OBJDIR) :
	mkdir -p $@

clean:
	rm -r obj/*
	rm main
