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






#define __fini                      PASTE2(__XALT__fini,                      MY_NAME)
#define __init                      PASTE2(__XALT__init,                      MY_NAME)
#define myfini                      PASTE2(__XALT_myfini,                     MY_NAME)
#define myinit                      PASTE2(__XALT_myinit,                     MY_NAME)
#define wrapper_for_fini            PASTE2(__XALT_wrapper_for_fini,           MY_NAME)

#define base64_decode               PASTE2(__XALT_base64_decode,              HIDE)
#define base64_encode               PASTE2(__XALT_base64_encode,              HIDE)
#define buildEnvT                   PASTE2(__XALT_buildEnvT,                  HIDE)
#define buildUserT                  PASTE2(__XALT_buildUserT,                 HIDE)
#define buildXALTRecordT            PASTE2(__XALT_buildXALTRecordT,           HIDE)
#define build_proc                  PASTE2(__XALT_build_proc,                 HIDE)
#define build_resultDir             PASTE2(__XALT_build_resultDir,            HIDE)
#define build_resultFn              PASTE2(__XALT_build_resultFn,             HIDE)
#define build_uuid                  PASTE2(__XALT_build_uuid,                 HIDE)
#define compute_sha1                PASTE2(__XALT_compute_sha1,               HIDE)
#define compress_string             PASTE2(__XALT_compress_string,            HIDE)
#define create_xalt_tmpdir_str      PASTE2(__XALT_create_xalt_tmpdir_str,     HIDE)
#define epoch                       PASTE2(__XALT_epoch,                      HIDE)
#define extractXALTRecordString     PASTE2(__XALT_extractXALTRecordString,    HIDE)
#define filterEnvT                  PASTE2(__XALT_filterEnvT,                 HIDE)
#define free_proc                   PASTE2(__XALT_free_proc,                  HIDE)
#define hostname_parser             PASTE2(__XALT_hostname_parser,            HIDE)
#define hostname_parser_cleanup     PASTE2(__XALT_hostname_parser_cleanup,    HIDE)
#define init_proc                   PASTE2(__XALT_init_proc,                  HIDE)
#define insert_key_double           PASTE2(__XALT_insert_key_double,          HIDE)
#define insert_key_string           PASTE2(__XALT_insert_key_string,          HIDE)
#define json_add_S2D                PASTE2(__XALT_json_add_S2D,               HIDE)
#define json_add_S2S                PASTE2(__XALT_json_add_S2S,               HIDE)
#define json_add_SET                PASTE2(__XALT_json_add_SET,               HIDE)
#define json_add_json_str           PASTE2(__XALT_json_add_json_str,          HIDE)
#define json_add_ptA                PASTE2(__XALT_json_add_ptA,               HIDE)
#define json_fini                   PASTE2(__XALT_json_fini,                  HIDE)
#define json_init                   PASTE2(__XALT_json_init,                  HIDE)
#define keep_env_name               PASTE2(__XALT_keep_env_name,              HIDE)
#define keep_path                   PASTE2(__XALT_keep_path,                  HIDE)
#define mkpath                      PASTE2(__XALT_mkpath,                     HIDE)
#define my_hostname_parser          PASTE2(__XALT_my_hostname_parser,         HIDE)
#define my_hostname_parser_cleanup  PASTE2(__XALT_my_hostname_parser_cleanup, HIDE)
#define parseProcMaps               PASTE2(__XALT_parseProcMaps,              HIDE)
#define path_parser_cleanup         PASTE2(__XALT_path_parser_cleanup,        HIDE)
#define proc_cmdline                PASTE2(__XALT_proc_cmdline,               HIDE)
#define remove_xalt_tmpdir          PASTE2(__XALT_remove_xalt_tmpdir,         HIDE)
#define run_submission              PASTE2(__XALT_run_submission,             HIDE)
#define translate                   PASTE2(__XALT_translate,                  HIDE)
#define transmit                    PASTE2(__XALT_transmit,                   HIDE)
#define walkProcessTree             PASTE2(__XALT_walkProcessTree,            HIDE)
#define xalt_dir                    PASTE2(__XALT_dir,                        HIDE)
#define xalt_fgets_alloc            PASTE2(__XALT_fgets_alloc,                HIDE)
#define xalt_quotestring            PASTE2(__XALT_quotestring,                HIDE)
#define xalt_quotestring_free       PASTE2(__XALT_quotestring_free,           HIDE)
#define xalt_syshost                PASTE2(__XALT_syshost,                    HIDE)
#define xalt_unquotestring          PASTE2(__XALT_unquotestring,              HIDE)
#define xalt_vendor_note            PASTE2(__XALT_vendor_note,                HIDE)
//
//
//// Data
//
//#define hostnameA                   PASTE2(__XALT_hostnameA,                  MY_NAME)
//#define hostnameSz                  PASTE2(__XALT_hostnameSz,                 MY_NAME)
//#define pathPatternA                PASTE2(__XALT_pathPatternA,               MY_NAME)
//#define pathPatternSz               PASTE2(__XALT_pathPatternSz,              MY_NAME)
//#define envPatternA                 PASTE2(__XALT_envPatternA,                MY_NAME)
//#define envPatternSz                PASTE2(__XALT_envPatternSz,               MY_NAME)
//#define scalar_rangeA               PASTE2(__XALT_scalar_rangeA,              MY_NAME)
//#define scalar_rangeSz              PASTE2(__XALT_scalar_rangeSz,             MY_NAME)
//#define mpi_rangeA                  PASTE2(__XALT_mpi_rangeA,                 MY_NAME)
//#define mpi_rangeSz                 PASTE2(__XALT_mpi_rangeSz,                MY_NAME)
//#define mpi_always_record           PASTE2(__XALT_mpi_always_record,          MY_NAME)
//
//#define background                  PASTE2(__XALT_background,                 MY_NAME)
//#define countA                      PASTE2(__XALT_countA,                     MY_NAME)
//#define end_time                    PASTE2(__XALT_end_time,                   MY_NAME)
//#define errfd                       PASTE2(__XALT_errfd,                      MY_NAME)
//#define exec_path                   PASTE2(__XALT_exec_path,                  MY_NAME)
//#define ldLibPathArg                PASTE2(__XALT_ldLibPathArg,               MY_NAME)
//#define my_rank                     PASTE2(__XALT_my_rank,                    MY_NAME)
//#define my_size                     PASTE2(__XALT_my_size,                    MY_NAME)
//#define my_syshost                  PASTE2(__XALT_my_syshost,                 MY_NAME)
//#define pathArg                     PASTE2(__XALT_pathArg,                    MY_NAME)
//#define ppid                        PASTE2(__XALT_ppid,                       MY_NAME)
//#define reject_flag                 PASTE2(__XALT_reject_flag,                MY_NAME)
//#define start_time                  PASTE2(__XALT_start_time,                 MY_NAME)
//#define usr_cmdline                 PASTE2(__XALT_usr_cmdline,                MY_NAME)
//#define xalt_reasonA                PASTE2(__XALT_reasonA,                    MY_NAME)
//#define xalt_tracing                PASTE2(__XALT_tracing,                    MY_NAME)


#endif //XALT_OBFUSCATE
