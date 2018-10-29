SRC  := clear.c compare.c copy.c gen_uuid.c isnull.c pack.c parse.c predefined.c test_uuid.c unpack.c \
        unparse.c uuid_time.c
OBJS := $(patsubst %.c %.o $(SRC))

LIBUUID := libuuid.a

$(LIBUUID) : $(OBJS)
	ar ru $@ $^
	ranlib $@


clear.o: clear.c uuidP.h
compare.o: compare.c uuidP.h
copy.o: copy.c uuidP.h
isnull.o: isnull.c uuidP.h
pack.o: pack.c uuidP.h
parse.o: parse.c uuidP.h
predefined.o: predefined.c uuidP.h
unpack.o: unpack.c uuidP.h
unparse.o: unparse.c uuidP.h
uuid_time.o:  uuid_time.c uuidP.h

