EXEC := try
OBJS := try.o msg.o

all: $(EXEC)

$(EXEC) : $(OBJS)
	gfortran -o $@ $^

try.o: try.f90
	gfortran -c $^

msg.o: msg.f90
	gfortran -c $^

neat:
	$(RM) *~
clean: neat
	$(RM) $(OBJS)
clobber: clean
	$(RM) $(EXEC)



