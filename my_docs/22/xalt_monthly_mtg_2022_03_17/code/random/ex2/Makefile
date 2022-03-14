EXEC    := try
TARGETS := $(EXEC) libxalt.so
OBJS := try.o msg.o

all: $(TARGETS)

$(EXEC) : $(OBJS)
	gfortran -o $@ $^

try.o: try.f90
	gfortran -fno-underscoring -c $^

msg.o: msg.f90
	gfortran -fno-underscoring -c $^

libxalt.so: xalt.o
	$(LINK.c) -o $@ -fPIC -shared $^

neat:
	$(RM) *~
clean: neat
	$(RM) $(OBJS)
clobber: clean
	$(RM) $(EXEC)

test:	all
	echo; ./try; echo
	LD_PRELOAD=./libxalt.so  ./try  ; echo

