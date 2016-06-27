#ifndef XALT_FUNCTION_NAMES
#define XALT_FUNCTION_NAMES

#define PASTE(a,b)  PaStE2(a,b)
#define PaStE2(a,b) a##b

#ifndef HIDE
#define HIDE _xalt_1_0
#endif


#define abspath            PASTE(__XALT_abspath,HIDE)
#define build_uuid_str     PASTE(__XALT_build_uuid_str,HIDE)
#define compute_value      PASTE(__XALT_compute_value,HIDE)
#define hostname           PASTE(__XALT_hostname,HIDE)
#define myfini             PASTE(__XALT_myfini,HIDE)
#define myinit             PASTE(__XALT_myinit,HIDE)
#define reject             PASTE(__XALT_reject,HIDE)
#define xalt_fgets_alloc   PASTE(__XALT_fgets_alloc,HIDE)
#define xalt_quotestring   PASTE(__XALT_quotestring,HIDE)
#define xalt_syshost       PASTE(__XALT_syshost,HIDE)
#define xalt_unquotestring PASTE(__XALT_unquotestring,HIDE)

#endif //XALT_FUNCTION_NAMES
