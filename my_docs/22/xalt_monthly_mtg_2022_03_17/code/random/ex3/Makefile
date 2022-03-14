EXEC := try
OBJS := try.o random.o
CC      := gcc
TARGETS := try libxalt.so

all: $(TARGETS)

try: try.o random.o
	gfortran -o $@ $^

libxalt.so: xalt.o
	$(LINK.c) -o $@ -fPIC -shared $^

xalt.o: xalt.c
	$(COMPILE.c) -o $@ -fPIC -c $^

try.o: try.f90
	gfortran -fno-underscoring -c $^
random.o: random.f90
	gfortran -fno-underscoring -c $^




neat:
	$(RM) *~
clean: neat
	$(RM) *.o
clobber: clean
	$(RM) $(TARGETS)

test:	all
	echo; ./try; echo
	LD_PRELOAD=./libxalt.so  ./try  ; echo

