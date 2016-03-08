CXX := g++
CC  := gcc

CXXFLAGS := -g -O0 -std=c++11
CFLAGS   := -g -O0
EXEC  	 := xalt_run_submission
SRC   	 := main.C Options.C capture.C extractXALTRecord.C parseLDD.C buildEnvT.C buildUserT.C Json.C
C_SRC 	 := quotestring.c
OBJS  	 := $(patsubst %.C, %.o, $(SRC)) $(patsubst %.c, %.o, $(C_SRC))

all: $(EXEC)

$(EXEC) : $(OBJS)
	$(LINK.C) -o $@ $^

run:
	./$(EXEC) --start 1456589662.2041 --end 1456589672.3041 --syshost mijo --exec $(EXEC) --ntasks 1 --uuid $$(uuidgen) '[ "./parse", "--start", "1.0"]' >  Run.json
	python -mjson.tool Run.json > R2.json



neat:
	$(RM) *~
clean: neat $
	$(RM) -r *.o $(EXEC).dSYM
clobber: clean
	$(RM) $(EXEC)

main.o: main.C Options.h run_submission.h
Options.o: Options.C Options.h  xalt_config.h
buildEnvT.o: buildEnvT.C run_submission.h
buildUserT.o: buildUserT.C run_submission.h
capture.o: capture.C run_submission.h
extractXALTRecord.o: extractXALTRecord.C run_submission.h xalt_config.h
parseLDD.o: parseLDD.C run_submission.h xalt_config.h
Json.o: Json.C run_submission.h Json.h
