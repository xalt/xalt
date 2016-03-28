CXX := g++
CC  := gcc

LIB_OPTIONS       := -fPIC -shared
ifeq ($(OS_NAME),Darwin)
  LIB_OPTIONS     := -bundle -undefined dynamic_lookup
endif

CXX_LDFLAGS := $(shell mysql_config --libs)
CF_INIT     := -I$(THIS_DIR)/compiled/ -fPIC
CXXFLAGS    := -g -O0 -std=c++11 -I$(THIS_DIR)/compiled/ $(shell mysql_config --cflags)
CFLAGS      := -g -O0 
EXEC  	    := $(DESTDIR)$(LIBEXEC)/xalt_run_submission
SRC   	    := main.C Options.C capture.C extractXALTRecord.C parseLDD.C buildEnvT.C buildUserT.C Json.C \
              base64.C zstring.C direct2db.C ConfigParser.C translate.C Process.C
C_SRC 	    := xalt_quotestring.c xalt_fgets_alloc.c jsmn.c
OBJS  	    := $(patsubst %.C, %.o, $(SRC)) $(patsubst %.c, %.o, $(C_SRC))

all: ECHO $(EXEC) build_init $(DESTDIR)$(LIBEXEC)/xalt_syshost $(DESTDIR)$(LIBEXEC)/xalt_generate_assembly

ECHO:
	@echo XALT_EXECUTABLE_TRACKING: $(XALT_EXECUTABLE_TRACKING)

$(EXEC) : $(OBJS)
	$(LINK.C) -o $@ $^ -lz $(CXX_LDFLAGS)

$(DESTDIR)$(LIBEXEC)/xalt_syshost: xalt_syshost.o xalt_fgets_alloc.o
	gcc -DHAVE_MAIN -O2 -o $@ $<

$(DESTDIR)$(LIBEXEC)/xalt_generate_assembly: xalt_generate_assembly.o Process.o
	gcc -O2 -o $@ $<


build_init: 
	gcc $(CF_INIT) -o $(DESTDIR)$(LIBEXEC)/xalt_quotestring.o -c xalt_quotestring.c
	gcc $(CF_INIT) -o $(DESTDIR)$(LIBEXEC)/xalt_syshost.o -c xalt_syshost.c
	gcc $(CF_INIT) -o $(DESTDIR)$(LIBEXEC)/xalt_fgets_alloc.o -c xalt_fgets_alloc.c
	gcc $(CF_INIT) -DSTATE=LD_PRELOAD -o $(DESTDIR)$(LIBEXEC)/xalt_initialize.o -c xalt_initialize.c; 
	gcc $(CF_INIT) $(LIB_OPTIONS) -o $(DESTDIR)$(LIBEXEC)/libxalt_init.so $(DESTDIR)$(LIBEXEC)/xalt_initialize.o $(DESTDIR)$(LIBEXEC)/xalt_syshost.o $(DESTDIR)$(LIBEXEC)/xalt_quotestring.o -luuid; 
	gcc $(CF_INIT) -g -O0 -DSTATE=LD_PRELOAD -DFULL_TRACING -o $(DESTDIR)$(LIBEXEC)/xalt_initialize.o -c xalt_initialize.c; 
	gcc $(CF_INIT) -g -O0 $(LIB_OPTIONS) -o $(DESTDIR)$(LIBEXEC)/libxalt_init_dbg.so $(DESTDIR)$(LIBEXEC)/xalt_initialize.o $(DESTDIR)$(LIBEXEC)/xalt_syshost.o $(DESTDIR)$(LIBEXEC)/xalt_quotestring.o -luuid; 
	gcc $(CF_INIT) -DSTATE=REGULAR    -o $(DESTDIR)$(LIBEXEC)/xalt_initialize.o -c xalt_initialize.c; 

run:
	./$(EXEC) --start 1456589662.2041 --end 1456589672.3041 --syshost mijo --exec $(EXEC) --ntasks 1 --uuid $$(uuidgen) '[ "./parse", "--start", "1.0"]' >  Run.json
	python -mjson.tool Run.json > R2.json



neat:
	$(RM) *~
clean: 
	$(RM) -r *.o $(notdir $(EXEC)).dSYM

main.o: main.C Options.h run_submission.h base64.h zstring.h $(THIS_DIR)/compiled/xalt_config.h capture.h
Json.o: Json.C run_submission.h Json.h xalt_quotestring.h  capture.h
Options.o: Options.C Options.h  $(THIS_DIR)/compiled/xalt_config.h
buildEnvT.o: buildEnvT.C run_submission.h  capture.h
buildUserT.o: buildUserT.C run_submission.h  capture.h
capture.o: capture.C run_submission.h  capture.h
extractXALTRecord.o: extractXALTRecord.C run_submission.h $(THIS_DIR)/compiled/xalt_config.h  capture.h
parseLDD.o: parseLDD.C run_submission.h $(THIS_DIR)/compiled/xalt_config.h  capture.h
xalt_quotestring.o: xalt_quotestring.c xalt_quotestring.h
base64.o: base64.C base64.h
zstring.o: zstring.C zstring.h
direct2db.o: direct2db.C run_submission.h ConfigParser.h xalt_regex.h Json.h  capture.h
ConfigParser.o: ConfigParser.C ConfigParser.h $(THIS_DIR)/compiled/xalt_config.h base64.h
xalt_fgets_alloc.o: xalt_fgets_alloc.c xalt_fgets_alloc.h
jsmn.o: jsmn.c jsmn.h
buildRmapT.o: buildRmapT.C $(THIS_DIR)/compiled/xalt_config.h run_submission.h xalt_quotestring.h xalt_fgets_alloc.h  capture.h
translate.o: translate.C run_submission.h  capture.h
Process.o: Process.C Process.h xalt_fgets_alloc.h 
xalt_syshost.o: xalt_syshost.c xalt_fgets_alloc.h
xalt_generate_assembly.o: xalt_generate_assembly.C $(THIS_DIR)/compiled/xalt_config.h Process.h

