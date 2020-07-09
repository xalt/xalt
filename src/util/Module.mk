local_dir     	 := util

local_c_src   	 := base64.c            \
	            buildXALTRecordT.c  \
		    build_uuid.c        \
	            capture.c           \
		    compute_sha1.c      \
		    epoch.c             \
                    extractMain.c       \
		    transmit.c          \
		    xalt_c_utils.c      \
		    xalt_dir.c          \
		    xalt_epoch.c        \
		    xalt_fgets_alloc.c  \
		    xalt_quotestring.c  \
		    xalt_record_pkg.c   \
		    xalt_tmpdir.c       \
		    zstring.c
local_c_src   	 := $(addprefix $(local_dir)/, $(local_c_src))

local_cxx_src 	 := test_record_pkg.C            \
                    xalt_configuration_report.C
local_cxx_src  	 := $(addprefix $(local_dir)/, $(local_cxx_src))

EPOCH_X  	 := $(DESTDIR)$(LIBEXEC)/xalt_epoch
EPOCH_C_SRC      := xalt_epoch.c epoch.c
EPOCH_C_SRC      := $(addprefix $(local_dir)/, $(EPOCH_C_SRC))
EPOCH_OBJ        := $(patsubst %.c, %.o, $(EPOCH_C_SRC))

EXTR_REC_X       := $(DESTDIR)$(LIBEXEC)/xalt_extract_record.x
EXTR_REC_C_SRC   := extractMain.c buildXALTRecordT.c capture.c
EXTR_REC_C_SRC   := $(addprefix $(local_dir)/, $(EXTR_REC_C_SRC))
EXTR_REC_OBJ     := $(patsubst %.c, %.o, $(EXTR_REC_C_SRC))


c_sources     += $(local_c_src) 
cxx_sources   += $(local_cxx_source)
programs      += $(EXTR_REC_X) $(EPOCH_X) 


$(EPOCH_X) : $(EPOCH_OBJ)
	$(LINK.c) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^

$(EXTR_REC_X) : $(EXTR_REC_OBJ)
	$(LINK.c) $(OPTLVL) $(WARN_FLAGS) $(LDFLAGS) -o $@ $^
