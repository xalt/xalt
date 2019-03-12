CC     := gcc
EXEC   := walk_so
SO     := libwalk.so
CFLAGS := -g -O0


all: $(EXEC) $(SO)

$(EXEC): main.o xalt.o
	$(LINK.c) -o $@ $^

$(SO): walk_so.o
	$(LINK.c) -fPIC -shared -o $@ $^

walk_so.o: walk_so.c
	$(COMPILE.c) -fPIC -o $@ -c $<

main.o: main.c
	$(COMPILE.c) -o $@ -c $<

xalt.o: xalt.s
	as -o $@ $<
new.o: new.s
	as -o $@ $<

neat:
	$(RM) *~
clean: neat
	$(RM) walk_so.o xalt.o new.o main.o
clobber: clean
	$(RM) $(EXEC) $(SO)
