local_dir     	 := util

local_c_src   	 := base64.c            \
	            buildJson.c         \
	            buildXALTRecordT.c  \
		    build_uuid.c        \
	            capture.c           \
		    compute_sha1.c      \
		    epoch.c             \
                    extractMain.c       \
	            insert.c            \
	            process.c           \
		    transmit.c          \
		    xalt_c_utils.c      \
		    xalt_dir.c          \
		    xalt_epoch.c        \
		    xalt_fgets_alloc.c  \
		    xalt_quotestring.c  \
		    xalt_record_pkg.c   \
		    xalt_tmpdir.c       \
		    xalt_vendor_note.c  \
		    zstring.c
local_c_src   	 := $(addprefix $(local_dir)/, $(local_c_src))

local_cxx_src 	 := test_record_pkg.C            \
                    xalt_configuration_report.C
local_cxx_src  	 := $(addprefix $(local_dir)/, $(local_cxx_src))

EPOCH_X  	 := $(DESTDIR)$(LIBEXEC)/xalt_epoch
EPOCH_C	         := xalt_epoch.c epoch.c
EPOCH_C          := $(addprefix $(local_dir)/, $(EPOCH_C))
EPOCH_OBJ        := $(patsubst %.c, %.o, $(EPOCH_C))

EXTR_REC_X       := $(DESTDIR)$(LIBEXEC)/xalt_extract_record.x
EXTR_REC_C       := extractMain.c buildXALTRecordT.c capture.c
EXTR_REC_C       := $(addprefix $(local_dir)/, $(EXTR_REC_C))
EXTR_REC_OBJ     := $(patsubst %.c, %.o, $(EXTR_REC_C))

CFG_RPT_X        := $(DESTDIR)$(LIBEXEC)/xalt_configuration_report.x
CFG_RPT_CXX      := xalt_configuration_report.C
CFG_RPT_CXX      := $(addprefix $(local_dir)/, $(CFG_RPT_CXX))
CFG_RPT_C        := epoch.c capture.c xalt_dir.c buildJson.c xalt_quotestring.c
CFG_RPT_C        := $(addprefix $(local_dir)/, $(CFG_RPT_C)) __build__/xalt_syshost.c
CFG_RPT_OBJ      := $(patsubst %.C, %.o, $(CFG_RPT_CXX)) \
                    $(patsubst %.c, %.o, $(CFG_RPT_C))

TST_REC_PKG_X    := $(DESTDIR)$(LIBEXEC)/test_record_pkg
TST_REC_PKG_CXX  := test_record_pkg.C
TST_REC_PKG_CXX  := $(addprefix $(local_dir)/, $(TST_REC_PKG_CXX))
TST_REC_PKG_OBJ  := $(patsubst %.C, %.o, $(TST_REC_PKG_CXX))

REC_PKG_X        := $(DESTDIR)$(LIBEXEC)/xalt_record_pkg
REC_PKG_C        := xalt_record_pkg.c transmit.c xalt_c_utils.c xalt_quotestring.c build_uuid.c \
                    base64.c zstring.c xalt_fgets_alloc.c xalt_tmpdir.c xalt_dir.c
REC_PKG_C        := $(addprefix $(local_dir)/, $(REC_PKG_C)) __build__/xalt_syshost.c
REC_PKG_OBJ      := $(patsubst %.c, %.o, $(REC_PKG_C))

SYSHOST_X        := $(DESTDIR)$(SBIN)/xalt_syshost

#=====================================================================#
c_sources     	 += $(local_c_src) 
cxx_sources   	 += $(local_cxx_src)
programs      	 += $(EXTR_REC_X) $(EPOCH_X) $(CFG_RPT_X) $(REC_PKG_X) \
                    $(TST_REC_PKG_X) $(SYSHOST_X)
#=====================================================================#

$(EPOCH_X) : $(EPOCH_OBJ)
	$(LINK.c) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(EXTR_REC_X) : $(EXTR_REC_OBJ)
	$(LINK.c) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(CFG_RPT_X): $(CFG_RPT_OBJ)
	$(LINK.cc) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(REC_PKG_X): $(REC_PKG_OBJ)
	$(LINK.cc) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^ -lz -ldl -Wl,-rpath,$(LIB64) -L$(LIB64) -lcurl


$(TST_REC_PKG_X): $(TST_REC_PKG_OBJ)
	$(LINK.cc) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(SYSHOST_X): __build__/xalt_syshost_main.o util/xalt_fgets_alloc.o
	$(LINK.c) $(OPTLVL) $(WARN_FLAGS) -o $@ $<

__build__/xalt_syshost_main.o : $(CURDIR)/__build__/xalt_syshost.c util/xalt_fgets_alloc.h
	$(COMPILE.c) -I$(THIS_DIR) -c -DHAVE_MAIN $(OPTLVL)  $(WARN_FLAGS) $(LDFLAGS) -o $@ $<

