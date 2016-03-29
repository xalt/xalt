CXX := g++
CC  := gcc

LIB_OPTIONS       := -fPIC -shared
ifeq ($(OS_NAME),Darwin)
  LIB_OPTIONS     := -bundle -undefined dynamic_lookup
endif

CXX_LDFLAGS := $(shell mysql_config --libs)
CF_INIT     := -I$(THIS_DIR)/compiled/ -fPIC -DJSMN_PARENT_LINKS
CXXFLAGS    := -g -O0 -std=c++11 -I$(THIS_DIR)/compiled/ $(shell mysql_config --cflags) -DJSMN_PARENT_LINKS
CFLAGS      := -g -O0 
EXEC  	    := $(DESTDIR)$(LIBEXEC)/xalt_run_submission
SRC   	    := xalt_run_submission.C ConfigParser.C Json.C Options.C Process.C base64.C          \
               buildEnvT.C buildRmapT.C buildUserT.C capture.C direct2db.C extractXALTRecord.C   \
               parseLDD.C translate.C xalt_utils.C zstring.C 
C_SRC 	    := xalt_quotestring.c xalt_fgets_alloc.c jsmn.c
OBJS  	    := $(patsubst %.C, %.o, $(SRC)) $(patsubst %.c, %.o, $(C_SRC))

all: ECHO $(EXEC) build_init $(DESTDIR)$(LIBEXEC)/xalt_syshost           \
                             $(DESTDIR)$(LIBEXEC)/xalt_generate_assembly \
                             $(DESTDIR)$(LIBEXEC)/xalt_strip_linklib     \
                             $(DESTDIR)$(LIBEXEC)/xalt_generate_linkdata

ECHO:
	@echo XALT_EXECUTABLE_TRACKING: $(XALT_EXECUTABLE_TRACKING)

$(EXEC) : $(OBJS)
	$(LINK.C) -o $@ $^ -lz $(CXX_LDFLAGS)

$(DESTDIR)$(LIBEXEC)/xalt_syshost: xalt_syshost.o xalt_fgets_alloc.o
	gcc -O2 -o $@ $<

xalt_syshost.o: xalt_syshost.c xalt_fgets_alloc.h
	gcc -c -DHAVE_MAIN -O2 -o $@ $<

$(DESTDIR)$(LIBEXEC)/xalt_generate_assembly: xalt_generate_assembly.o extract_linker.o Process.o xalt_fgets_alloc.o
	g++ -O2 -o $@ $?

$(DESTDIR)$(LIBEXEC)/xalt_generate_linkdata: xalt_generate_linkdata.o extract_linker.o Process.o xalt_fgets_alloc.o xalt_quotestring.o buildRmapT.o jsmn.o xalt_utils.o
	g++ -O2 -o $@ $? $(CXX_LDFLAGS)

$(DESTDIR)$(LIBEXEC)/xalt_strip_linklib: xalt_strip_linklib.o xalt_quotestring.o buildRmapT.o jsmn.o xalt_utils.o xalt_fgets_alloc.o
	g++ -O2 -o $@ $?

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

ConfigParser.o: ConfigParser.C ConfigParser.h $(THIS_DIR)/compiled/xalt_config.h base64.h xalt_fgets_alloc.h xalt_utils.h
Json.o: Json.C Json.h xalt_quotestring.h xalt_types.h 
Options.o: Options.C Options.h capture.h $(THIS_DIR)/compiled/xalt_config.h xalt_types.h
Process.o: Process.C Process.h xalt_fgets_alloc.h 
base64.o: base64.C base64.h
buildEnvT.o: buildEnvT.C run_submission.h Options.h xalt_types.h
buildRmapT.o: buildRmapT.C $(THIS_DIR)/compiled/xalt_config.h buildRmapT.h Options.h xalt_types.h xalt_quotestring.h jsmn.h xalt_fgets_alloc.h xalt_utils.h
buildUserT.o: buildUserT.C run_submission.h Options.h xalt_types.h
capture.o: capture.C  capture.h xalt_types.h
direct2db.o: direct2db.C run_submission.h Options.h xalt_types.h ConfigParser.h xalt_regex.h Json.h xalt_utils.h
extract_linker.o: extract_linker.C extract_linker.h xalt_types.h Process.h
extractXALTRecord.o: extractXALTRecord.C run_submission.h Options.h xalt_types.h $(THIS_DIR)/compiled/xalt_config.h capture.h
jsmn.o: jsmn.c jsmn.h
link_direct2db.o: link_direct2db.C link_submission.h xalt_types.h ConfigParser.h Json.h
parseLDD.o: parseLDD.C run_submission.h Options.h xalt_types.h $(THIS_DIR)/compiled/xalt_config.h capture.h
parseLDTrace.o: parseLDTrace.C link_submission.h xalt_types.h xalt_gets_alloc.h $(THIS_DIR)/compiled/xalt_config.h
translate.o: translate.C run_submission.h Options.h xalt_types.h
xalt_fgets_alloc.o: xalt_fgets_alloc.c xalt_fgets_alloc.h
xalt_generate_assembly.o: xalt_generate_assembly.C extract_linker.h $(THIS_DIR)/compiled/xalt_config.h
xalt_generate_linkdata.o: xalt_generate_linkdata.C extract_linker.h $(THIS_DIR)/compiled/xalt_config.h xalt_types.h base64.h zstring.h capture.h
xalt_quotestring.o: xalt_quotestring.c xalt_quotestring.h
xalt_run_submission.o: xalt_run_submission.C run_submission.h Options.h xalt_types.h base64.h zstring.h $(THIS_DIR)/compiled/xalt_config.h capture.h buildRmapT.h
xalt_strip_linklib.o: xalt_strip_linklib.C xalt_types.h buildRmapT.h
xalt_utils.o: xalt_utils.C xalt_utils.h xalt_types.h $(THIS_DIR)/compiled/xalt_config.h
zstring.o: zstring.C zstring.h

