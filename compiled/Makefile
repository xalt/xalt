CXX := g++
CC  := gcc

LIB_OPTIONS       := -fPIC -shared
ifeq ($(OS_NAME),Darwin)
  LIB_OPTIONS     := -bundle -undefined dynamic_lookup
endif

CF_INIT     := 
CXXFLAGS    := -g -O0 -std=c++11
CFLAGS      := -g -O0
EXEC  	    := $(DESTDIR)$(LIBEXEC)/xalt_run_submission
SRC   	    := main.C Options.C capture.C extractXALTRecord.C parseLDD.C buildEnvT.C buildUserT.C Json.C
C_SRC 	    := quotestring.c
OBJS  	    := $(patsubst %.C, %.o, $(SRC)) $(patsubst %.c, %.o, $(C_SRC))

all: $(EXEC) build_init $(DESTDIR)$(LIBEXEC)/xalt_syshost

$(EXEC) : $(OBJS)
	$(LINK.C) -o $@ $^

$(DESTDIR)$(LIBEXEC)/xalt_syshost: xalt_syshost.c
	gcc -DHAVE_MAIN -O2 -o $@ $<

build_init: 
	gcc $(CF_INIT) -fPIC -o $(DESTDIR)$(LIBEXEC)/xalt_syshost.o -c xalt_syshost.c
	gcc $(CF_INIT) -DSTATE=LD_PRELOAD -fPIC -o $(DESTDIR)$(LIBEXEC)/xalt_initialize.o -c xalt_initialize.c; 
	gcc $(CF_INIT) $(LIB_OPTIONS) -o $(DESTDIR)$(LIBEXEC)/libxalt_init.so $(DESTDIR)$(LIBEXEC)/xalt_initialize.o $(DESTDIR)$(LIBEXEC)/xalt_syshost.o -luuid; 
	gcc $(CF_INIT) -g -O0 -DSTATE=LD_PRELOAD -DFULL_TRACING -fPIC -o $(DESTDIR)$(LIBEXEC)/xalt_initialize.o -c xalt_initialize.c; 
	gcc $(CF_INIT) -g -O0 $(LIB_OPTIONS) -o $(DESTDIR)$(LIBEXEC)/libxalt_init_dbg.so $(DESTDIR)$(LIBEXEC)/xalt_initialize.o $(DESTDIR)$(LIBEXEC)/xalt_syshost.o -luuid; 
	gcc $(CF_INIT) -DSTATE=REGULAR    -o $(DESTDIR)$(LIBEXEC)/xalt_initialize.o -c xalt_initialize.c; 

run:
	./$(EXEC) --start 1456589662.2041 --end 1456589672.3041 --syshost mijo --exec $(EXEC) --ntasks 1 --uuid $$(uuidgen) '[ "./parse", "--start", "1.0"]' >  Run.json
	python -mjson.tool Run.json > R2.json



neat:
	$(RM) *~
clean: 
	$(RM) -r *.o $(notdir $(EXEC)).dSYM

main.o: main.C Options.h run_submission.h
Json.o: Json.C run_submission.h Json.h quotestring.h
Options.o: Options.C Options.h  xalt_config.h
buildEnvT.o: buildEnvT.C run_submission.h
buildUserT.o: buildUserT.C run_submission.h
capture.o: capture.C run_submission.h
extractXALTRecord.o: extractXALTRecord.C run_submission.h xalt_config.h
parseLDD.o: parseLDD.C run_submission.h xalt_config.h
quotestring.o: quotestring.c quotestring.h
