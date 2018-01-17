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
#define abspath                 PASTE2(__XALT_abspath,                 MY_NAME)
#define build_uuid              PASTE2(__XALT_build_uuid,              MY_NAME)
#define compute_value           PASTE2(__XALT_compute_value,           MY_NAME)
#define hostname                PASTE2(__XALT_hostname,                MY_NAME)
#define myfini                  PASTE2(__XALT_myfini,                  MY_NAME)
#define myinit                  PASTE2(__XALT_myinit,                  MY_NAME)
#define reject                  PASTE2(__XALT_reject,                  MY_NAME)
#define keep_path               PASTE2(__XALT_keep_path,               MY_NAME)
#define path_parser_cleanup     PASTE2(__XALT_path_parser_cleanup,     MY_NAME)
#define hostname_parser         PASTE2(__XALT_hostname_parser,         MY_NAME)
#define hostname_parser_cleanup PASTE2(__XALT_hostname_parser_cleanup, MY_NAME)
#define xalt_fgets_alloc        PASTE2(__XALT_fgets_alloc,             HIDE)
#define xalt_quotestring        PASTE2(__XALT_quotestring,             HIDE)
#define xalt_quotestring_free   PASTE2(__XALT_quotestring_free,        HIDE)
#define xalt_syshost            PASTE2(__XALT_syshost,                 HIDE)
#define xalt_unquotestring      PASTE2(__XALT_unquotestring,           HIDE)

// Data

#define hostnameA               PASTE2(__XALT_hostnameA,               MY_NAME)
#define hostnameSz              PASTE2(__XALT_hostnameSz,              MY_NAME)
#define pathPatternA            PASTE2(__XALT_pathPatternA,            MY_NAME)
#define pathPatternSz           PASTE2(__XALT_pathPatternSz,           MY_NAME)
#define envPatternA             PASTE2(__XALT_envPatternA,             MY_NAME)
#define envPatternSz            PASTE2(__XALT_envPatternSz,            MY_NAME)

#define background              PASTE2(__XALT_background,              MY_NAME)
#define countA                  PASTE2(__XALT_countA,                  MY_NAME)
#define end_time                PASTE2(__XALT_end_time,                MY_NAME)
#define errfd                   PASTE2(__XALT_errfd,                   MY_NAME)
#define exec_path               PASTE2(__XALT_exec_path,               MY_NAME)
#define ldLibPathArg            PASTE2(__XALT_ldLibPathArg,            MY_NAME)
#define my_rank                 PASTE2(__XALT_my_rank,                 MY_NAME)
#define my_size                 PASTE2(__XALT_my_size,                 MY_NAME)
#define my_syshost              PASTE2(__XALT_my_syshost,              MY_NAME)
#define pathArg                 PASTE2(__XALT_pathArg,                 MY_NAME)
#define ppid                    PASTE2(__XALT_ppid,                    MY_NAME)
#define reject_flag             PASTE2(__XALT_reject_flag,             MY_NAME)
#define start_time              PASTE2(__XALT_start_time,              MY_NAME)
#define usr_cmdline             PASTE2(__XALT_usr_cmdline,             MY_NAME)
#define uuid_str                PASTE2(__XALT_uuid_str,                MY_NAME)
#define xalt_reasonA            PASTE2(__XALT_reasonA,                 MY_NAME)
#define xalt_tracing            PASTE2(__XALT_tracing,                 MY_NAME)


#endif //XALT_OBFUSCATE
