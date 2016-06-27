EXEC      := try
C_SRC     := xalt_quotestring.c
CXX_SRC   := try.C
OBJS      := try.o xalt_quotestring.o
CFLAGS    := -g -O0
CXXFLAGS  := -g -O0

$(EXEC): $(OBJS)
	$(LINK.C) -o $@ $^

try.o: try.C xalt_quotestring.h
xalt_quotestring.o: xalt_quotestring.c xalt_quotestring.h
