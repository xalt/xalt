#ifndef XALT_OBFUSCATE
#define XALT_OBFUSCATE

#define PASTE2(a,b)    PaStE2(a,b)
#define PaStE2(a,b)    a##b
#define PASTE3(a,b,c)  PaStE3(a,b,c)
#define PaStE3(a,b,c)  a##b##c

#ifndef HIDE
#define HIDE _xalt_1_5
#endif

#define UNDERSCORE         _
#define MY_NAME            PASTE3(UNDERSCORE,STATE,HIDE)

// Functions
#define abspath            PASTE2(__XALT_abspath,HIDE)
#define build_uuid_str     PASTE2(__XALT_build_uuid_str,HIDE)
#define compute_value      PASTE2(__XALT_compute_value,HIDE)
#define hostname           PASTE2(__XALT_hostname,HIDE)
#define myfini             PASTE2(__XALT_myfini,MY_NAME)
#define myinit             PASTE2(__XALT_myinit,MY_NAME)
#define reject             PASTE2(__XALT_reject,HIDE)
#define xalt_fgets_alloc   PASTE2(__XALT_fgets_alloc,HIDE)
#define xalt_quotestring   PASTE2(__XALT_quotestring,HIDE)
#define xalt_syshost       PASTE2(__XALT_syshost,HIDE)
#define xalt_unquotestring PASTE2(__XALT_unquotestring,HIDE)

// Data

#define acceptPathA        PASTE2(__XALT_acceptPathA,HIDE)
#define ignorePathA        PASTE2(__XALT_ignorePathA,HIDE)
#define hostnameA          PASTE2(__XALT_hostnameA,HIDE)
#define acceptEnvA         PASTE2(__XALT_acceptEnvA,HIDE)
#define ignoreEnvA         PASTE2(__XALT_ignoreEnvA,HIDE)

#define acceptPathSz       PASTE2(__XALT_acceptPathSz,HIDE)
#define ignorePathSz       PASTE2(__XALT_ignorePathSz,HIDE)
#define hostnameSz         PASTE2(__XALT_hostnameSz,HIDE)
#define acceptEnvSz        PASTE2(__XALT_acceptEnvSz,HIDE)
#define ignoreEnvSz        PASTE2(__XALT_ignoreEnvSz,HIDE)



#endif //XALT_OBFUSCATE
