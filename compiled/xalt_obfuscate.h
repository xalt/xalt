#ifndef XALT_OBFUSCATE
#define XALT_OBFUSCATE

#define PASTE(a,b)  PaStE2(a,b)
#define PaStE2(a,b) a##b

#ifndef HIDE
#define HIDE _xalt_1_5
#endif

// Functions
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

// Data

#define acceptPathA        PASTE(__XALT_acceptPathA,HIDE)
#define ignorePathA        PASTE(__XALT_ignorePathA,HIDE)
#define hostnameA          PASTE(__XALT_hostnameA,HIDE)
#define acceptEnvA         PASTE(__XALT_acceptEnvA,HIDE)
#define ignoreEnvA         PASTE(__XALT_ignoreEnvA,HIDE)

#define acceptPathSz       PASTE(__XALT_acceptPathSz,HIDE)
#define ignorePathSz       PASTE(__XALT_ignorePathSz,HIDE)
#define hostnameSz         PASTE(__XALT_hostnameSz,HIDE)
#define acceptEnvSz        PASTE(__XALT_acceptEnvSz,HIDE)
#define ignoreEnvSz        PASTE(__XALT_ignoreEnvSz,HIDE)



#endif //XALT_OBFUSCATE
