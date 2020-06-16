CC      := gcc
TARGETS := try libxalt.so libxalt2.so

all: $(TARGETS)

try: try.c
	$(LINK.c) -o $@ $^

libxalt.so: xalt.o
	$(LINK.c) -o $@ -fPIC -shared $^

libxalt2.so: xalt2.o
	$(LINK.c) -o $@ -fPIC -shared $^

xalt.o: xalt.c
	$(COMPILE.c) -o $@ -fPIC -c $^

xalt2.o: xalt2.c
	$(COMPILE.c) -o $@ -fPIC -c $^

neat:
	$(RM) *~
clean: neat
	$(RM) *.o
clobber: clean
	$(RM) $(TARGETS)

test:	all
	echo; .try; echo
	LD_PRELOAD=./libxalt.so  ./try  ; echo
	LD_PRELOAD=./libxalt.so  ls     ; echo
	LD_PRELOAD=./libxalt2.so ls     ; echo

