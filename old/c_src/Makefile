CC   := gcc
EXEC := read_conf

SRC  := read_conf.c dictionary.c iniparser.c cdecode.c
OBJ  := $(patsubst %.c, %.o, $(SRC))

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@  $(OBJ)

read_conf.o: read_conf.c iniparser.h b64/cdecode.h
dictionary.o: dictionary.h dictionary.c
iniparser.o: iniparser.c iniparser.h dictionary.h
cdecode.o: cdecode.c b64/cdecode.h

neat: 
	$(RM) .*~ *~
clean: neat
	$(RM) *.o
clobber: clean
	$(RM) $(EXEC)
