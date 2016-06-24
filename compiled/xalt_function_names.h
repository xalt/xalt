#ifndef XALT_FUNCTION_NAMES
#define XALT_FUNCTION_NAMES

#define PASTE(a,b)  PaStE2(a,b)
#define PaStE2(a,b) a##b

#ifndef HIDE
#define HIDE _xalt_1_0
#endif


#define abspath            PASTE(XALT_abspath,HIDE)
#define build_uuid_str     PASTE(XALT_build_uuid_str,HIDE)
#define compute_value      PASTE(XALT_compute_value,HIDE)
#define hostname           PASTE(XALT_hostname,HIDE)
#define myfini             PASTE(XALT_myfini,HIDE)
#define myinit             PASTE(XALT_myinit,HIDE)
#define reject             PASTE(XALT_reject,HIDE)
#define xalt_fgets_alloc   PASTE(XALT_fgets_alloc,HIDE)
#define xalt_quotestring   PASTE(XALT_quotestring,HIDE)
#define xalt_syshost       PASTE(XALT_syshost,HIDE)
#define xalt_unquotestring PASTE(XALT_unquotestring,HIDE)

#endif //XALT_FUNCTION_NAMES
