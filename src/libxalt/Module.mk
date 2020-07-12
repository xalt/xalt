ifneq ($(MY_HOSTNAME_PARSER),no)
   HAVE_EXTERNAL_HOSTNAME_PARSER := -DHAVE_EXTERNAL_HOSTNAME_PARSER
   PARSER_TYPE                   := $(firstword $(subst :, ,$(MY_HOSTNAME_PARSER)))
   ifeq ($(PARSER_TYPE),c_file)
      MY_HOSTNAME_PARSER_SRC     := $(lastword  $(subst :, ,$(MY_HOSTNAME_PARSER)))
      MY_HOSTNAME_PARSER_OBJ     := $(DESTDIR)$(LIB64)/my_hostname_parser.o
      MY_HOSTNAME_PARSER_OBJ_32  := $(DESTDIR)$(LIB)/my_hostname_parser_32.o
   endif
   ifeq ($(PARSER_TYPE),library)
      MY_HOSTNAME_PARSER_LIBRARY := $(word  2, $(subst :, ,$(MY_HOSTNAME_PARSER)))
      MY_HOSTNAME_PARSER_LIB32   := $(word  3, $(subst :, ,$(MY_HOSTNAME_PARSER)))
      MY_HOSTNAME_PARSER_TARGET  := my_hostname_parser_target
      ifneq ($(MY_HOSTNAME_PARSER_LIB32),)
         MY_HOSTNAME_PARSER_TARGET  += my_hostname_parser_target_32
      endif
      MY_HOSTNAME_PARSER_OBJ     := $(DESTDIR)$(LIB64)/my_hostname_parser.a
      MY_HOSTNAME_PARSER_OBJ_32  := $(DESTDIR)$(LIB)/my_hostname_parser_32.a
   endif
endif

local_dir     	  := libxalt

local_c_src   	  :=                      \
		     buildEnvT.c          \
		     buildUserT.c         \
		     parseProcMaps.c      \
		     run_submission.c     \
		     translate.c          \
		     walkProcessTree.c    \
		     xalt_initialize.c 
local_c_src   	  := $(addprefix $(local_dir)/, $(local_c_src))

LIBXALT_INIT_64   := $(DESTDIR)$(LIB64)/libxalt_init.so
LIBXALT_OBJ_64    := libxalt/xalt_initialize_preload.o   \
	             libxalt/run_submission.o            \
                     libxalt/buildEnvT.o                 \
                     libxalt/buildUserT.o                \
                     libxalt/parseProcMaps.o             \
                     libxalt/translate.o                 \
                     libxalt/walkProcessTree.o           \
                     util/xalt_quotestring.o             \
                     __build__/lex.__XALT_path.o         \
                     __build__/lex.__XALT_host.o         \
		     util/base64.o                     	 \
	             util/build_uuid.o                   \
		     util/capture.o                   	 \
	             util/buildXALTRecordT.o             \
	             util/buildJson.o                    \
	             util/process.o                      \
                     util/xalt_dir.o                   	 \
                     util/xalt_tmpdir.o                	 \
                     util/xalt_vendor_note.o           	 \
                     util/xalt_fgets_alloc.o             \
                     $(MY_HOSTNAME_PARSER_OBJ)

XALT_INIT_D       := xalt_obfuscate.h xalt_types.h insert.h base64.h       \
                     xalt_quotestring.h xalt_header.h xalt_config.h        \
                     xalt_dir.h xalt_path_parser.h  xalt_hostname_parser.h \
                     xalt_tmpdir.h xalt_vendor_note.h build_uuid.h         \
                     buildXALTRecordT.h


#=====================================================================#
c_sources     	  += $(local_c_src) 
libraries         += $(LIBXALT_INIT_64)
extra_clean       += libxalt/xalt_initialize_preload.o
#=====================================================================#

__build__/lex.xalt_env.c: $(CURDIR)/__build__/xalt_env_parser.lex
	flex -P xalt_env -o $@ $^
__build__/lex.xalt_env.o: __build__/lex.xalt_env.c
	$(COMPILE.c) -Wno-unused-function  -o $@ $^

__build__/lex.__XALT_path.c: $(CURDIR)/__build__/xalt_path_parser.lex
	flex -P __XALT_path -o $@ $^
__build__/lex.__XALT_path.o: __build__/lex.__XALT_path.c xalt_obfuscate.h
	$(COMPILE.c)  -Wno-unused-function $(CF_INIT) -o $@ -c $<

__build__/lex.__XALT_host.c: $(CURDIR)/__build__/xalt_hostname_parser.lex
	flex -P __XALT_host -o $@ $^
__build__/lex.__XALT_host.o: __build__/lex.__XALT_host.c xalt_obfuscate.h
	$(COMPILE.c)  -Wno-unused-function $(CF_INIT) -o $@ -c $<


$(LIBXALT_INIT_64): $(LIBXALT_OBJ_64)
	$(LINK.c) $(CFLAGS) $(CF_INIT) $(LIB_OPTIONS) $(LDFLAGS) -L$(DESTDIR)$(LIB64) -o $@  $^ $(LIBDCGM) $(LIBNVML) -ldl

libxalt/xalt_initialize_preload.o: libxalt/xalt_initialize.c $(XALT_INIT_D)
	$(LINK.c) $(CFLAGS) $(CF_INIT) -Wno-unused-variable -DSTATE=LD_PRELOAD -DIDX=0 -o $@ -c $<

