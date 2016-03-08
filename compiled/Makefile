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

tags:	build_tags
	$(RM) file_list.*

build_tags:
	find . \( -regex '.*~$$\|.*/\.git\|.*/\.git/' -prune \)  \
               -o -type f > file_list.1
	sed -e 's|.*/.git.*||g'                                  \
            -e 's|.*/rt/.*/t1/.*||g'                             \
            -e 's|.*/html/.*$$||g'                               \
            -e 's|./TAGS||g'                                     \
            -e 's|./makefile||g'                                 \
            -e 's|./configure$$||g'                              \
            -e 's|.*\.DS_Store$$||g'                             \
            -e 's|.*\.tgz$$||g'                                  \
            -e 's|.*\.tar\.gz$$||g'                              \
            -e 's|.*\.tar\.bz2$$||g'                             \
            -e 's|.*\.csv$$||g'                                  \
	    -e 's|.*\.aux$$||g'                                  \
	    -e 's|.*\.fdb_latexmk$$||g'                          \
	    -e 's|.*\.fls$$||g'                                  \
	    -e 's|.*\.nav$$||g'                                  \
	    -e 's|.*\.out$$||g'                                  \
	    -e 's|.*\.pdf$$||g'                                  \
	    -e 's|.*\.pyc$$||g'                                  \
	    -e 's|.*db\.conf$$||g'                               \
	    -e 's|.*\.pyo$$||g'                                  \
	    -e 's|.*\.snm$$||g'                                  \
	    -e 's|.*\.toc$$||g'                                  \
	    -e 's|.*\.vrb$$||g'                                  \
	    -e 's|.*\.key$$||g'                                  \
            -e 's|^#.*||g'                                       \
            -e 's|.*/#.*||g'                                     \
            -e 's|\.#.*||g'                                      \
            -e 's|.*/\.#.*||g'                                   \
            -e 's|.*\.pdf$$||g'                                  \
            -e 's|.*\.used$$||g'                                 \
            -e 's|./.*\.log$$||g'                                \
            -e 's|./testreports/.*||g'                           \
            -e 's|./docs/build/.*||g'                            \
            -e 's|./config\.status$$||g'                         \
            -e 's|.*\~$$||g'                                     \
            -e 's|./file_list\..*||g'                            \
            -e '/^\s*$$/d'                                       \
	       < file_list.1 > file_list.2
	etags  `cat file_list.2`

main.o: main.C Options.h run_submission.h
Options.o: Options.C Options.h
buildEnvT.o: buildEnvT.C run_submission.h
buildUserT.o: buildUserT.C run_submission.h
capture.o: capture.C run_submission.h
extractXALTRecord.o: extractXALTRecord.C run_submission.h
parseLDD.o: parseLDD.C run_submission.h
Json.o: Json.C run_submission.h Json.h
