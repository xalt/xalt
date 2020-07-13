ifneq ($(MY_HOSTNAME_PARSER),no)
   HAVE_EXTERNAL_HOSTNAME_PARSER := -DHAVE_EXTERNAL_HOSTNAME_PARSER
   PARSER_TYPE                   := $(firstword $(subst :, ,$(MY_HOSTNAME_PARSER)))
   ifeq ($(PARSER_TYPE),c_file)
      MY_HOSTNAME_PARSER_SRC     := $(lastword  $(subst :, ,$(MY_HOSTNAME_PARSER)))
      MY_HOSTNAME_PARSER_OBJ     := $(DESTDIR)$(LIB64)/my_hostname_parser.o
      MY_HOSTNAME_PARSER_O32     := $(DESTDIR)$(LIB)/my_hostname_parser.o32
   endif
   ifeq ($(PARSER_TYPE),library)
      MY_HOSTNAME_PARSER_LIBRARY := $(word  2, $(subst :, ,$(MY_HOSTNAME_PARSER)))
      MY_HOSTNAME_PARSER_LIB32   := $(word  3, $(subst :, ,$(MY_HOSTNAME_PARSER)))
      MY_HOSTNAME_PARSER_TARGET  := my_hostname_parser_target
      ifneq ($(MY_HOSTNAME_PARSER_LIB32),)
         MY_HOSTNAME_PARSER_TARGET  += my_hostname_parser_target_32
      endif
      MY_HOSTNAME_PARSER_OBJ     := $(DESTDIR)$(LIB64)/my_hostname_parser.a
      MY_HOSTNAME_PARSER_O32     := $(DESTDIR)$(LIB)/my_hostname_parser_32.a
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
RUN_SUBMISSION_OBJ:= libxalt/run_submission.o            \
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

LIBXALT_INIT_32   := $(DESTDIR)$(LIB)/libxalt_init.so
RUN_SUBMISSION_O32:= libxalt/run_submission.o32          \
                     libxalt/buildEnvT.o32               \
                     libxalt/buildUserT.o32              \
                     libxalt/parseProcMaps.o32           \
                     libxalt/translate.o32               \
                     libxalt/walkProcessTree.o32         \
                     util/xalt_quotestring.o32           \
                     __build__/lex.__XALT_path.o32       \
                     __build__/lex.__XALT_host.o32       \
		     util/base64.o32                   	 \
	             util/build_uuid.o32                 \
		     util/capture.o32                 	 \
	             util/buildXALTRecordT.o32           \
	             util/buildJson.o32                  \
	             util/process.o32                    \
                     util/xalt_dir.o32                 	 \
                     util/xalt_tmpdir.o32              	 \
                     util/xalt_vendor_note.o32         	 \
                     util/xalt_fgets_alloc.o32           \
                     $(MY_HOSTNAME_PARSER_O32)

RUN_SUBMISSION_LIB:= $(DESTDIR)$(LIB64)/librun_submission.a
XALT_INIT_REGULAR := $(DESTDIR)$(LIB64)/xalt_initialize_regular.o
LIBXALT_OBJ_64    := libxalt/xalt_initialize_preload.o   \
	             $(RUN_SUBMISSION_OBJ)

RUN_SUBMISSION_L32:= $(DESTDIR)$(LIB)/librun_submission.a
XALT_INIT_REG32   := $(DESTDIR)$(LIB)/xalt_initialize_regular.o32
LIBXALT_O32       := libxalt/xalt_initialize_preload.o32   \
	             $(RUN_SUBMISSION_O32)

XALT_INIT_D       := xalt_obfuscate.h xalt_types.h insert.h base64.h       \
                     xalt_quotestring.h xalt_header.h xalt_config.h        \
                     xalt_dir.h xalt_path_parser.h  xalt_hostname_parser.h \
                     xalt_tmpdir.h xalt_vendor_note.h build_uuid.h         \
                     buildXALTRecordT.h

BUILD_INIT_32bit_no  :=
BUILD_INIT_32bit_yes := $(XALT_INIT_REG32) $(RUN_SUBMISSION_L32) $(LIBXALT_INIT_32) 



#=====================================================================#
c_sources     	  += $(local_c_src) 
libraries         += $(XALT_INIT_REGULAR) $(LIBXALT_INIT_64) $(RUN_SUBMISSION_LIB) \
                     $(BUILD_INIT_32bit_yes)
extra_clean       += libxalt/xalt_initialize_preload.o   libxalt/xalt_initialize_regular.o   \
                     libxalt/xalt_initialize_preload.o32 libxalt/xalt_initialize_regular.o32 \

#=====================================================================#

__build__/lex.xalt_env.c: $(CURDIR)/__build__/xalt_env_parser.lex
	flex -P xalt_env -o $@ $^
__build__/lex.__XALT_path.c: $(CURDIR)/__build__/xalt_path_parser.lex
	flex -P __XALT_path -o $@ $^
__build__/lex.__XALT_host.c: $(CURDIR)/__build__/xalt_hostname_parser.lex
	flex -P __XALT_host -o $@ $^

__build__/lex.__XALT_path.o: __build__/lex.__XALT_path.c xalt_obfuscate.h
	$(COMPILE.c)  -Wno-unused-function $(CF_INIT) -o $@ -c $<
__build__/lex.xalt_env.o: __build__/lex.xalt_env.c
	$(COMPILE.c) -Wno-unused-function  -o $@ $^
__build__/lex.__XALT_host.o: __build__/lex.__XALT_host.c xalt_obfuscate.h
	$(COMPILE.c)  -Wno-unused-function $(CF_INIT) -o $@ -c $<


$(LIBXALT_INIT_64): $(LIBXALT_OBJ_64)
	$(LINK.c) $(CFLAGS) $(CF_INIT) $(LIB_OPTIONS) $(LDFLAGS) -L$(DESTDIR)$(LIB64) -o $@  $^ $(LIBDCGM) $(LIBNVML) -ldl

libxalt/xalt_initialize_preload.o: libxalt/xalt_initialize.c $(XALT_INIT_D)
	$(COMPILE.c) $(CF_INIT) -Wno-unused-variable -DSTATE=LD_PRELOAD -DIDX=0 -o $@ -c $<

$(XALT_INIT_REGULAR): libxalt/xalt_initialize.c $(XALT_INIT_D)
	$(COMPILE.c) $(CF_INIT) -Wno-unused-variable -DSTATE=REGULAR -DIDX=1 -o $@ -c $<
$(RUN_SUBMISSION_LIB): $(RUN_SUBMISSION_OBJ)
	$(AR) $(ARFLAGS) $@ $^

#----------------------------------------------------------------------
#  32 bit 
#----------------------------------------------------------------------

$(LIBXALT_INIT_32): $(LIBXALT_O32)
	$(LINK.c) -m32 $(CFLAGS) $(CF_INIT) $(LIB_OPTIONS) $(LDFLAGS) -L$(DESTDIR)$(LIB64) -o $@  $^ $(LIBDCGM) $(LIBNVML) -ldl

libxalt/xalt_initialize_preload.o32: libxalt/xalt_initialize.c $(XALT_INIT_D)
	$(COMPILE.c) $(CPPFLAGS) -m32 $(CF_INIT) -Wno-unused-variable -DSTATE=LD_PRELOAD -DIDX=0 -o $@ -c $<

$(XALT_INIT_REG32): libxalt/xalt_initialize.c $(XALT_INIT_D)
	$(COMPILE.c) -m32 $(CF_INIT) -Wno-unused-variable -DSTATE=REGULAR    -DIDX=1 -o $@ -c $<

__build__/lex.__XALT_path.o32: __build__/lex.__XALT_path.c xalt_obfuscate.h
	$(COMPILE.c)  -m32 -Wno-unused-function $(CF_INIT) -o $@ -c $<
__build__/lex.xalt_env.o32: __build__/lex.xalt_env.c
	$(COMPILE.c)  -m32 -Wno-unused-function  -o $@ $^
__build__/lex.__XALT_host.o32: __build__/lex.__XALT_host.c xalt_obfuscate.h
	$(COMPILE.c)  -m32 -Wno-unused-function $(CF_INIT) -o $@ -c $<

$(RUN_SUBMISSION_L32): $(RUN_SUBMISSION_O32)
	$(AR) $(ARFLAGS) $@ $^

