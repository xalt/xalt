local_dir     	  := linker

local_c_src   	  :=                                  \
                     jsmn.c                           \
                     my_uuidgen.c                     \
                     xalt_realpath.c
local_c_src   	  := $(addprefix $(local_dir)/, $(local_c_src))

local_cxx_src 	  :=                                  \
	             buildRmapT.C                     \
	             parseJsonStr.C                   \
                     parseLDTrace.C                   \
                     xalt_extract_linker.C            \
                     xalt_generate_linkdata.C         \
                     xalt_generate_watermark.C        \
                     xalt_rmap_exists.C               \
                     xalt_strip_linklib.C             \
                     xalt_utils.C
local_cxx_src  	  := $(addprefix $(local_dir)/, $(local_cxx_src))


UUID_GEN_X        := $(DESTDIR)$(LIBEXEC)/my_uuidgen
UUID_GEN_C        := my_uuidgen.c
UUID_GEN_C        := $(addprefix $(local_dir)/, $(UUID_GEN_C)) util/build_uuid.c util/xalt_dir.c
UUID_GEN_OBJ      := $(patsubst %.c, %.o, $(UUID_GEN_C))

EXTR_LNKR_X       := $(DESTDIR)$(LIBEXEC)/xalt_extract_linker
EXTR_LNKR_CXX     := xalt_extract_linker.C
EXTR_LNKR_CXX     := $(addprefix $(local_dir)/, $(EXTR_LNKR_CXX))
EXTR_LNKR_C       := util/process.c util/buildJson.c util/xalt_fgets_alloc.c util/xalt_quotestring.c
EXTR_LNKR_OBJ     := $(patsubst %.C, %.o, $(EXTR_LNKR_CXX)) \
                     $(patsubst %.c, %.o, $(EXTR_LNKR_C))

GEN_WTRMK_X       := $(DESTDIR)$(LIBEXEC)/xalt_generate_watermark
GEN_WTRMK_CXX     := xalt_generate_watermark.C parseJsonStr.C
GEN_WTRMK_CXX     := $(addprefix $(local_dir)/, $(GEN_WTRMK_CXX))
GEN_WTRMK_C       := linker/jsmn.c util/epoch.c util/xalt_vendor_note.c util/xalt_quotestring.c \
                     util/insert.c
GEN_WTRMK_OBJ     := $(patsubst %.C, %.o, $(GEN_WTRMK_CXX)) \
                     $(patsubst %.c, %.o, $(GEN_WTRMK_C))

REALPATH_X        := $(DESTDIR)$(LIBEXEC)/xalt_realpath
REALPATH_C        := linker/xalt_realpath.c
REALPATH_OBJ      := $(patsubst %.c, %.o, $(REALPATH_C))

LNKDATA_X         := $(DESTDIR)$(LIBEXEC)/xalt_generate_linkdata
LNKDATA_CXX       := xalt_generate_linkdata.C parseJsonStr.C parseLDTrace.C
LNKDATA_CXX       := $(addprefix $(local_dir)/, $(LNKDATA_CXX))
LNKDATA_C         := linker/jsmn.c util/insert.c util/xalt_fgets_alloc.c util/buildJson.c    \
                     util/xalt_quotestring.c util/xalt_c_utils.c util/transmit.c             \
                     util/base64.c util/zstring.c
LNKDATA_OBJ       := $(patsubst %.C, %.o, $(LNKDATA_CXX)) \
                     $(patsubst %.c, %.o, $(LNKDATA_C))

RMAP_EXISTS_X     := $(DESTDIR)$(LIBEXEC)/xalt_rmap_exists
RMAP_EXISTS_CXX   := xalt_rmap_exists.C xalt_utils.C
RMAP_EXISTS_CXX   := $(addprefix $(local_dir)/, $(RMAP_EXISTS_CXX))
RMAP_EXISTS_OBJ   := $(patsubst %.C, %.o, $(RMAP_EXISTS_CXX))

STRIP_LNKLIB_X   := $(DESTDIR)$(LIBEXEC)/xalt_strip_linklib
STRIP_LNKLIB_CXX := xalt_strip_linklib.C parseJsonStr.C buildRmapT.C xalt_utils.C
STRIP_LNKLIB_CXX := $(addprefix $(local_dir)/, $(STRIP_LNKLIB_CXX))
STRIP_LNKLIB_C   := linker/jsmn.c util/insert.c util/xalt_quotestring.c util/xalt_fgets_alloc.c
STRIP_LNKLIB_OBJ := $(patsubst %.C, %.o, $(STRIP_LNKLIB_CXX)) \
                     $(patsubst %.c, %.o, $(STRIP_LNKLIB_C))

#=====================================================================#
c_sources     	  += $(local_c_src) 
cxx_sources   	  += $(local_cxx_src)
programs      	  += $(UUID_GEN_X) $(EXTR_LNKR_X) $(GEN_WTRMK_X)      \
                     $(REALPATH_X) $(LNKDATA_X) $(RMAP_EXISTS_X)      \
                     $(STRIP_LNKLIB_X)
#=====================================================================#

$(REALPATH_X) : $(REALPATH_OBJ)
	$(LINK.c) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(UUID_GEN_X) : $(UUID_GEN_OBJ)
	$(LINK.c) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^ -ldl

$(EXTR_LNKR_X) : $(EXTR_LNKR_OBJ)
	$(LINK.cc) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(GEN_WTRMK_X) : $(GEN_WTRMK_OBJ)
	$(LINK.cc) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(LNKDATA_X) : $(LNKDATA_OBJ)
	$(LINK.cc) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^ -lz -ldl -Wl,-rpath,$(LIB64) -L$(LIB64) -lcurl

$(RMAP_EXISTS_X) : $(RMAP_EXISTS_OBJ)
	$(LINK.cc) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(STRIP_LNKLIB_X) : $(STRIP_LNKLIB_OBJ)
	$(LINK.cc) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^


