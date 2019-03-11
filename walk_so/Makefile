CC   := gcc
EXEC := walk_so



$(EXEC): walk_so.o xalt.o new.o ogrt.o
	$(LINK.c) -o $@ $^

walk_so.o: walk_so.c
	$(COMPILE.c) -o $@ -c $<

xalt.o: xalt.s
	as -o $@ $<
new.o: new.s
	as -o $@ $<

neat:
	$(RM) *~
clean: neat
	$(RM) walk_so.o xalt.o new.o
clobber: clean
	$(RM) $(EXEC)
